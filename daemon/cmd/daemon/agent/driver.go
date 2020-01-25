package agent

import (
	"os"

	"github.com/vishvananda/netlink/nl"
	"github.com/vishvananda/netns"
	"golang.org/x/sys/unix"
)

type Driver struct {
	socket *nl.NetlinkSocket
	configLoader ConfigLoader
	isConnected bool
}

type RecivedMessage struct {
	Type uint16
	Data []byte
}

type DriverCommunicator interface {
	Connect() error
	DisConnect()
	GetIsConnectiong() bool
	SendMessage(essage Message) error
	ReciveMessage() ([]RecivedMessage, error)
}

func NewDriver(configLoader ConfigLoader) (*Driver, error) {
	soc, err := nl.GetNetlinkSocketAt(netns.None(), netns.None(), 17)
	if err != nil {
		return nil, err
	}
	return &Driver{soc, configLoader, false}, nil
}

func (driver *Driver) Connect() error {
	name := driver.configLoader.GetConfig().DriverName
	mess :=   CreateAccount{name}
	err := driver.SendMessage(&mess)
	if err != nil {
		return err
	}
	res, err := driver.ReciveMessage()
	if err != nil {
		return err
	}
	resultMessage := res[0]
	if resultMessage.Type != (&CreateAccountResult{}).GetMessageTypeId(){
		return &os.LinkError{}
	}
	result := DeserializeCreateAccountResult(resultMessage.Data)
	if result.Name != name || result.Result != 0{
		return &os.LinkError{}
	}
	driver.isConnected = true;
	return nil
}

func (driver *Driver) DisConnect() {

}
func (driver *Driver) GetIsConnectiong() bool{
	return driver.isConnected
}
func (driver *Driver) SendMessage(message Message) error {
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
	err := driver.socket.Send(req)
	return err

}
func (driver *Driver) ReciveMessage() ([]RecivedMessage, error) {
	msgs, _, err := driver.socket.Receive()
	if err != nil {
		return nil, err
	}
	var result []RecivedMessage

	for _, msg := range msgs {
		result = append(result, RecivedMessage{msg.Header.Type, msg.Data})
	}
	return result, nil
}
