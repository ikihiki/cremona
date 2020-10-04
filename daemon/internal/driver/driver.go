package driver

import (
	"errors"

	"github.com/ikihiki/cremona/daemon/internal/driver/message"
)

func GetDeviceHealth(connection *Connection, deviceID uint32) (bool, error) {
	if !connection.GetIsConnected() {
		return false, errors.New("not connected")
	}

	err := connection.SendMessage(&message.GetHealth{DeviceID: deviceID})
	if err != nil {
		return false, err
	}

	result, err := connection.ReciveMessage()
	if err != nil {
		return false, err
	}

	if result.Type != (&message.GetHealthResult{}).GetMessageTypeId() {
		return false, errors.New("invslid message")
	}

	m, err := message.DeserializeGetHealthResult(result.Data)
	if err != nil {
		return false, err
	}
	return m.Status, nil
}
