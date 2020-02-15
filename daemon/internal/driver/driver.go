package driver

import "github.com/ikihiki/cremona/daemon/internal/message"

import "errors"

type Driver struct {
	connection *Connection
}

func NewDriver(port int) (*Driver, error) {
	connection := NewConnection(port)
	err := connection.Connect()
	if err != nil {
		return nil, err
	}
	return &Driver{connection: connection}, nil
}

func (driver *Driver) GetStats() (*message.GetStatsResult, error) {
	if !driver.connection.GetIsConnected() {
		return nil, errors.New("not connected")
	}

	err := driver.connection.SendMessage(&message.GetStats{})
	if err != nil {
		return nil, err
	}

	result, err :=driver.connection.ReciveMessage()
	if err != nil {
		return nil, err
	}
	if len(result) != 1 || result[0].Type != (&message.GetStatsResult{}).GetMessageTypeId() {
		return nil, errors.New("invslid message")
	}

	return message.DeserializeGetStatsResult(result[0].Data)
}
