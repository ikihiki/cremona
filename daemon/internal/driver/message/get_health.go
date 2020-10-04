package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type GetHealth struct {
	DeviceID uint32
}

func (this *GetHealth) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 2
}
func (this *GetHealth) GetFlags() uint16 {
	return unix.NLM_F_REQUEST
}

func (message *GetHealth) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.DeviceID})
	return buff.Len()
}

func (message *GetHealth) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.DeviceID})
	return buff.Bytes()
}

type GetHealthResult struct {
	DeviceID uint32
	Status bool
}

func (this *GetHealthResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 3
}

func DeserializeGetHealthResult(data []byte) (*GetHealthResult, error) {
	buff := new(bytes.Buffer)
	_, err := buff.Write(data)
	if err != nil {
		return nil, err
	}
	decoder := msgpack.NewDecoder(buff)
	result := new(GetHealthResult)
	err = decoder.Decode(result)
	if err != nil {
		return nil, err
	}
	return result, nil
}
