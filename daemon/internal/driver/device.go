package driver

import (
	"fmt"

	"github.com/ikihiki/cremona/daemon/internal/message"
)

type DeviceCallbacks interface {
	NewToot(msg *message.NewToot) *message.NewTootResult
	SendToot(msg *message.SendToot) *message.SendTootResult
}

type Device struct {
	Id        uint64
	driver    *Driver
	callbacks DeviceCallbacks
}

func (device *Device) DestroyDevice() error {
	return device.driver.destroyDevice(device)
}

func (device *Device) ProcessMessages() error {

	result, _ := device.driver.connection.ReciveMessage()
	fmt.Printf("%#v\n", result)

	switch result.Type {
	case message.GetNewTootMessageTypeId():
		newToot, err := message.DeserializeNewToot(result.Data)
		if err != nil {
			return err
		}
		result := device.callbacks.NewToot(newToot)
		err = device.driver.connection.SendMessage(result)
		if err != nil {
			return err
		}
	case message.GetSendTootMessageTypeId():
		sendToot, err := message.DeserializeSendToot(result.Data)
		if err != nil {
			return err
		}
		result := device.callbacks.SendToot(sendToot)
		err = device.driver.connection.SendMessage(result)
		if err != nil {
			return err
		}

	}
	return nil
}
