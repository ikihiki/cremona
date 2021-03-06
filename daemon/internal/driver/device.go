package driver

import (
	"context"
	"errors"
	"fmt"
	"log"
	"sync"
	"time"

	"github.com/ikihiki/cremona/daemon/internal/driver/message"
	"github.com/ikihiki/cremona/daemon/internal/toot"
	"golang.org/x/sys/unix"
)

type Configrator interface {
	GetDeviceName() string
	GetUserId() uint32
}

type Device struct {
	Id         uint32
	config     Configrator
	connection *Connection
	tootManage toot.TootManager
	isNormal bool
	mutex sync.Mutex
}

func NewDevice(connection *Connection, config Configrator, tootManage toot.TootManager) (*Device, error) {
	if !connection.GetIsConnected() {
		return nil, errors.New("not connected")
	}

	name := config.GetDeviceName()
	uid := config.GetUserId()
	err := connection.SendMessage(&message.CreateDevice{Name: name, Uid: uid})
	if err != nil {
		return nil, err
	}

	result, err := connection.ReciveMessage()
	if err != nil {
		return nil, err
	}
	if result.Type == unix.NLMSG_ERROR {
		return nil, errors.New("something wrong")
	}

	if result.Type != (&message.CreateDeviceResult{}).GetMessageTypeId() {
		return nil, errors.New("invalid message")
	}

	message, err := message.DeserializeCreateAccountResult(result.Data)
	if err != nil {
		return nil, err
	}

	return &Device{connection: connection, config: config, Id: message.Id, tootManage: tootManage, isNormal: true}, nil
}



func (device *Device) DestroyDevice() error {
	if !device.connection.GetIsConnected() {
		return errors.New("not connected")
	}

	err := device.connection.SendMessage(&message.DestroyDevice{Id: device.Id})
	if err != nil {
		return err
	}

	result, err := device.connection.ReciveMessage()
	if err != nil {
		return err
	}

	if result.Type == unix.NLMSG_ERROR {

	}

	if result.Type != (&message.DestroyDeviceResult{}).GetMessageTypeId() {
		return errors.New("invalid message")
	}
	message, err := message.DeserializeDestroyDeviceResult(result.Data)
	if err != nil {
		return err
	}
	if message.Id != device.Id {
		return errors.New("invalid device")
	}
	return nil
}

func (device *Device)  SendGetDeviceHealth() error {
	device.mutex.Lock()
	if device.isNormal == false{
		device.mutex.Unlock()
		return errors.New("device is not respond");
	}
	device.isNormal = false;
	device.mutex.Unlock()
	if !device.connection.GetIsConnected() {
		return errors.New("not connected")
	}

	err := device.connection.SendMessage(&message.GetHealth{DeviceID: device.Id})
	if err != nil {
		return  err
	}
	return nil
}

func (device *Device) ProcessMessages() error {

	result, _ := device.connection.ReciveMessage()
	return device.processMessages(result)
}

func (device *Device) processNewtoot(recive *RecivedMessage) error {
	newToot, err := message.DeserializeNewToot(recive.Data)
	if err != nil {
		return err
	}
	rescode := 0

	err = device.tootManage.NewToot(newToot.TootId)

	if err != nil {
		log.Println(err)
		rescode = -1
	}
	result := &message.NewTootResult{TootId: newToot.TootId, DeviceId: newToot.DeviceId, Result: rescode}
	err = device.connection.SendMessage(result)
	return err
}

func (device *Device) processAddText(recive *RecivedMessage) error {
	addTootText, err := message.DeserializeAddTootElement(recive.Data)
	if err != nil {
		return err
	}
	len, err := device.tootManage.AddTootElement(addTootText.TootId, addTootText.Index, addTootText.Text)

	if err != nil {
		log.Println(err)

		len = -1
	}
	result := &message.AddTootElementResult{TootId: addTootText.TootId, DeviceId: addTootText.DeviceId, ElementId: addTootText.ElementId, Result: len}
	err = device.connection.SendMessage(result)
	return err
}

func (device *Device) processSendText(recive *RecivedMessage) error {
	sendToot, err := message.DeserializeSendToot(recive.Data)
	if err != nil {
		return err
	}

	err = device.tootManage.SendToot(sendToot.TootId)

	rescode := 0
	if err != nil {
		log.Println(err)
		rescode = -1
	}
	result := &message.SendTootResult{TootId: sendToot.TootId, DeviceId: sendToot.DeviceId, Result: rescode}
	err = device.connection.SendMessage(result)
	return err
}

func (device *Device) processHealth(recive *RecivedMessage) error {
	m, err := message.DeserializeGetHealthResult(recive.Data)
	if err != nil {
		return err
	}
	device.mutex.Lock()
	device.isNormal = m.Status;
	if device.isNormal == false{
		device.mutex.Unlock()
		return errors.New("device is unexpected");
	}
	device.mutex.Unlock()
	return nil
}


func (device *Device) processMessages(recive *RecivedMessage) error {

	switch recive.Type {
	case message.GetNewTootMessageTypeId():
		return device.processNewtoot(recive)
	case message.GetAddTootTextMessageTypeId():
		return device.processAddText(recive)
	case message.GetSendTootMessageTypeId():
		return device.processSendText(recive)
	default:
		return errors.New("Unknown message")
	}
}

func (device *Device) RunMessageLoop(c context.Context) error {
	messageChannel := make(chan *RecivedMessage)
	go func() {
		for {
			result, err := device.connection.ReciveMessage()
			if err != nil {
				fmt.Println(err)
			}
			messageChannel <- result
		}
	}()
	for {
		select {
		case <-c.Done():
			return nil;
		case <- time.Tick(time.Second * 10):
			err := device.SendGetDeviceHealth()
			if err != nil {
				return err
			}

		case message := <-messageChannel:
			err := device.processMessages(message)
			if err != nil {
				return err
			}
		}

	}
}
