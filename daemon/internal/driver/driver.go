package driver

import (
	"errors"

	"github.com/ikihiki/cremona/daemon/internal/driver/message"
)


func GetDriverStats(connection *Connection) (*message.GetStatsResult, error) {
	if !connection.GetIsConnected() {
		return nil, errors.New("not connected")
	}

	err := connection.SendMessage(&message.GetStats{})
	if err != nil {
		return nil, err
	}

	result, err := connection.ReciveMessage()
	if err != nil {
		return nil, err
	}

	if result.Type != (&message.GetStatsResult{}).GetMessageTypeId() {
		return nil, errors.New("invslid message")
	}

	return message.DeserializeGetStatsResult(result.Data)
}
