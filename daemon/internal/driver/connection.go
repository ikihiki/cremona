package driver

import (
	"os"
	"fmt"

	"github.com/vishvananda/netlink/nl"
	"github.com/vishvananda/netns"
	"golang.org/x/sys/unix"
)

type Message interface {
	GetMessageTypeId() uint16
	GetFlags() uint16
	Len() int
	Serialize() []byte
}

type RecivedMessage struct {
	Type uint16
	Data []byte
	Done bool
}

type Connection struct {
	protocol        int
	socket          *nl.NetlinkSocket
	isConnected     bool
	pendingMessages []*RecivedMessage
	inMulti         bool
}

func NewConnection(protocol int) *Connection {
	return &Connection{
		protocol:        protocol,
		isConnected:     false,
		pendingMessages: make([]*RecivedMessage, 0, 10),
		inMulti:         false,
	}

}

func (conn *Connection) Connect() error {
	socket, err := nl.GetNetlinkSocketAt(netns.None(), netns.None(), conn.protocol)
	if err != nil {
		return err
	}
	conn.socket = socket
	conn.isConnected = true
	return nil
}

func (conn *Connection) DisConnect() {
	conn.socket.Close()
	conn.socket = nil
	conn.isConnected = false
}

func (conn *Connection) GetIsConnected() bool {
	return conn.isConnected
}

func (conn *Connection) SendMessage(message Message) error {
	pid := os.Getpid()
	req := &nl.NetlinkRequest{
		NlMsghdr: unix.NlMsghdr{
			Len:   uint32(unix.SizeofNlMsghdr),
			Type:  message.GetMessageTypeId(),
			Pid:   uint32(pid),
			Flags: unix.NLM_F_REQUEST | message.GetFlags(),
		},
	}
	req.AddData(message)
	fmt.Printf("%#v\n", message)
	println(req.Serialize())
	err := conn.socket.Send(req)
	return err
}

func (conn *Connection) ReciveMessage() (*RecivedMessage, error) {
	if len(conn.pendingMessages) > 0 {
		message := conn.pendingMessages[0]
		conn.pendingMessages[0] = nil
		conn.pendingMessages = conn.pendingMessages[1:]
		return message, nil
	}

	msgs, _, err := conn.socket.Receive()
	if err != nil {
		return nil, err
	}

	for _, msg := range msgs {
		isMulti := (msg.Header.Flags & unix.NLM_F_MULTI) != 0
		if conn.inMulti {
			message := conn.pendingMessages[len(conn.pendingMessages)-1]
			if msg.Header.Type == unix.NLMSG_DONE {
				conn.inMulti = false
				message.Done = true
				continue
			}
			if !isMulti {
				conn.inMulti = false
				conn.pendingMessages = append(conn.pendingMessages, &RecivedMessage{
					Type: msg.Header.Type,
					Data: msg.Data,
					Done: true,
				})
				continue
			}
			if message.Type != msg.Header.Type {
				conn.pendingMessages = append(conn.pendingMessages, &RecivedMessage{
					Type: msg.Header.Type,
					Data: msg.Data,
					Done: false,
				})
				continue
			}

			message.Data = append(message.Data, msg.Data...)
		} else {
			if isMulti {
				conn.inMulti = true
				conn.pendingMessages = append(conn.pendingMessages, &RecivedMessage{
					Type: msg.Header.Type,
					Data: msg.Data,
					Done: false,
				})
			} else {
				conn.pendingMessages = append(conn.pendingMessages, &RecivedMessage{
					Type: msg.Header.Type,
					Data: msg.Data,
					Done: true,
				})
			}
		}
	}

	message := conn.pendingMessages[0]
	conn.pendingMessages[0] = nil
	conn.pendingMessages = conn.pendingMessages[1:]
	return message, nil
}

const (
	begin = iota
	multi
)
