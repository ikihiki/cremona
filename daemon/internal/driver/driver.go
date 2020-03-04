package driver

import (
	"errors"

	"github.com/ikihiki/cremona/daemon/internal/message"
	"golang.org/x/sys/unix"
)

type Driver struct {
	connection *Connection
	callbacks  DeviceCallbacks
}

func NewDriver(port int, callbacks DeviceCallbacks) (*Driver, error) {
	connection := NewConnection(port)
	err := connection.Connect()
	if err != nil {
		return nil, err
	}
	return &Driver{connection: connection, callbacks: callbacks}, nil
}

func (driver *Driver) GetStats() (*message.GetStatsResult, error) {
	if !driver.connection.GetIsConnected() {
		return nil, errors.New("not connected")
	}

	err := driver.connection.SendMessage(&message.GetStats{})
	if err != nil {
		return nil, err
	}

	result, err := driver.connection.ReciveMessage()
	if err != nil {
		return nil, err
	}

	if result.Type != (&message.GetStatsResult{}).GetMessageTypeId() {
		return nil, errors.New("invslid message")
	}

	return message.DeserializeGetStatsResult(result.Data)
}

func (driver *Driver) CreateDevice(name string) (*Device, error) {
	if !driver.connection.GetIsConnected() {
		return nil, errors.New("not connected")
	}

	err := driver.connection.SendMessage(&message.CreateDevice{Name: name})
	if err != nil {
		return nil, err
	}

	result, err := driver.connection.ReciveMessage()
	if err != nil {
		return nil, err
	}
	if result.Type == unix.NLMSG_ERROR {
		return nil, errors.New("something wrong")
	}

	if result.Type != (&message.CreateDeviceResult{}).GetMessageTypeId() {
		return nil, errors.New("invalid message")
	}

	message := message.DeserializeCreateAccountResult(result.Data)
	return &Device{driver: driver, Id: message.Id, callbacks: driver.callbacks}, nil
}

func (driver *Driver) destroyDevice(device *Device) error {
	if !driver.connection.GetIsConnected() {
		return errors.New("not connected")
	}

	err := driver.connection.SendMessage(&message.DestroyDevice{Id: device.Id})
	if err != nil {
		return err
	}

	result, err := driver.connection.ReciveMessage()
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
