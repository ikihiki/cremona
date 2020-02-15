package driver

import (
	"os"

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
}

type Connection struct {
	protocol    int
	socket      *nl.NetlinkSocket
	isConnected bool
}

func NewConnection(protocol int) *Connection {
	return &Connection{protocol: protocol}
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
	println(req.Serialize())
	err := conn.socket.Send(req)
	return err
}

func (conn *Connection) ReciveMessage() ([]RecivedMessage, error) {
	msgs, _, err := conn.socket.Receive()
	if err != nil {
		return nil, err
	}
	var result []RecivedMessage

	for _, msg := range msgs {
		result = append(result, RecivedMessage{msg.Header.Type & (^uint16(unix.NLMSG_DONE)), msg.Data})
	}
	return result, nil
}
