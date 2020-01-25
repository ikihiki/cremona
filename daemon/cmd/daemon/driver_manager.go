package main

import (
	"bytes"
	"errors"
	"fmt"
	"os"
	"strings"

	"github.com/google/uuid"
	"github.com/vishvananda/netlink/nl"
	"github.com/vishvananda/netns"
	"golang.org/x/sys/unix"
)

type NetLinkHandle struct {
	socket *nl.NetlinkSocket
	toot   map[uuid.UUID]*strings.Builder
}

func createNetlink() (*NetLinkHandle, error) {
	soc, err := nl.GetNetlinkSocketAt(netns.None(), netns.None(), 17)
	if err != nil {
		return nil, err
	}
	return &NetLinkHandle{
		socket: soc,
		toot:   make(map[uuid.UUID]*strings.Builder),
	}, nil
}

func (h *NetLinkHandle) sendMessage(typeId uint16, data nl.NetlinkRequestData, flags uint16) error {
	pid := os.Getpid()
	req := &nl.NetlinkRequest{
		NlMsghdr: unix.NlMsghdr{
			Len:   uint32(unix.SizeofNlMsghdr),
			Type:  typeId,
			Pid:   uint32(pid),
			Flags: unix.NLM_F_REQUEST | flags,
		},
	}
	req.AddData(data)
	println(req.Serialize())
	err := h.socket.Send(req)
	return err
}

func (h *NetLinkHandle) reciveMessage() ([]byte, error) {
	msgs, _, err := h.socket.Receive()
	if err != nil {
		return nil, err
	}
	buf := new(bytes.Buffer)
	for _, msg := range msgs {
		buf.Write(msg.Data)
	}
	return buf.Bytes(), nil
}

const CreateAccountTypeId = unix.NLMSG_MIN_TYPE + 25

func (h *NetLinkHandle) sendCreateConnection(data *CreateAccount) error {
	err := h.sendMessage(CreateAccountTypeId, data, unix.NLM_F_ACK)
	if err != nil {
		return err
	}
	res, err := h.reciveMessage()
	if err != nil {
		return err
	}
	fmt.Printf("%#v\n", res)
	return nil
}

const NewTootTypeId = unix.NLMSG_MIN_TYPE + 35
const NewTootResultTypeId = unix.NLMSG_MIN_TYPE + 45

func (h *NetLinkHandle) sendNewTootResult(data *NewTootResult) error {
	err := h.sendMessage(NewTootResultTypeId, data, 0)
	if err != nil {
		return err
	}
	return nil
}

const AddTootStringTypeId = unix.NLMSG_MIN_TYPE + 55
const AddTootStringResultTypeId = unix.NLMSG_MIN_TYPE + 65

func (h *NetLinkHandle) sendAddTootStringResult(data *AddTootStringResult) error {
	err := h.sendMessage(AddTootStringResultTypeId, data, 0)
	if err != nil {
		return err
	}
	return nil
}

const SendTootTypeId = unix.NLMSG_MIN_TYPE + 75
const SendTootResultTypeId = unix.NLMSG_MIN_TYPE + 85

func (h *NetLinkHandle) sendSendTootResult(data *SendTootResult) error {
	err := h.sendMessage(SendTootResultTypeId, data, 0)
	if err != nil {
		return err
	}
	return nil
}

func (h *NetLinkHandle) waitCommand() {
	for i := 0; i < 10; i++ {
		err := h.waitCommandIntenal()
		if err != nil {
			fmt.Printf("Error: %#v\n", err)
		}
	}
}

func (h *NetLinkHandle) waitCommandIntenal() error {
	msgs, _,err := h.socket.Receive()
	if err != nil {
		return err
	}
	for _, msg := range msgs {
		if msg.Header.Type == NewTootTypeId {
			fmt.Printf("Recive new toot Message %#v\n", msg)
			newToot, err := DeserializeNewToot(msg.Data)
			if err == nil {

				fmt.Printf("Recive new toot Message %#v\n", newToot)
				uuid, err := uuid.FromBytes(newToot.uuid[:])
				if err == nil {
					if _, ok := h.toot[uuid]; !ok {
						h.toot[uuid] = new(strings.Builder)
					} else {
						err = errors.New("Already Have uuid")
					}
				}
			}
			rescode := 0
			if err != nil {
				rescode = -1
			}
			result := NewTootResult{uuid: newToot.uuid, result: rescode}
			fmt.Printf("Send Message %#v\n", result)
			err = h.sendNewTootResult(&result)
			if err != nil {
				return err
			}
		}

		if msg.Header.Type == AddTootStringTypeId {
			fmt.Printf("Recive add toot Message %#v\n", msg)
			addToot, err := DeserializeAddTootString(msg.Data)
			if err == nil {
				fmt.Printf("Recive add toot Message %#v\n", addToot)
				uuid, err := uuid.FromBytes(addToot.uuid[:])
				if err == nil {
					if val, ok := h.toot[uuid]; ok {
						val.WriteString(addToot.text)
					} else {
						err = errors.New("No uuid")
					}
				}
			}
			rescode := len(addToot.text)
			if(err != nil){
				rescode = -1;
			}
			result := AddTootStringResult{uuid: addToot.uuid, result: rescode}
			fmt.Printf("Send Message %#v\n", result)
			err = h.sendAddTootStringResult(&result)
			if err != nil {
				return err
			}
		}

		if msg.Header.Type == SendTootTypeId{
			fmt.Printf("Recive send toot Message %#v\n", msg)
			sendToot, err := DeserializeSendToot(msg.Data)
			if err == nil {

				fmt.Printf("Recive send toot Message %#v\n", sendToot)
				uuid, err := uuid.FromBytes(sendToot.uuid[:])
				if err == nil {
					if val, ok := h.toot[uuid]; ok {
						toot := val.String();
						fmt.Printf("Send Toot %s \n", toot)
						//delete(h.toot, uuid)
					} else {
						err = errors.New("No uuid")
					}
				}
			}
			rescode := 0
			if err != nil {
				rescode = -1
			}
			result := SendTootResult{uuid: sendToot.uuid, result: rescode}
			fmt.Printf("Send Message %#v\n", result)
			err = h.sendSendTootResult(&result)
			if err != nil {
				return err
			}
		}
	}
	return nil
}
