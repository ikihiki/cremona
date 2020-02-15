package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type GetStats struct {
}

func (this *GetStats) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 2
}
func (this *GetStats) GetFlags() uint16 {
	return 0
}

func (message *GetStats) Len() int {
	return 0
}

func (message *GetStats) Serialize() []byte {
	return make([]byte, 0)
}

type GetStatsResult struct {
	TotalDeviceCount int
	TotalOpenToots   int
}

func (this *GetStatsResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 3
}

func DeserializeGetStatsResult(data []byte) (*GetStatsResult, error ){
	buff := new(bytes.Buffer)
	_, err := buff.Write(data)
	if err != nil {
		return nil, err
	}
	decoder := msgpack.NewDecoder(buff)
	result := new(GetStatsResult)
	err = decoder.Decode(result)
	if err != nil {
		return nil, err
	}
	return result, nil
}
