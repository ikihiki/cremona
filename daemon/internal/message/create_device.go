package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type CreateDevice struct{
	Name string
}

func (this *CreateDevice) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 4
}
func (this *CreateDevice) GetFlags() uint16 {
	return unix.NLM_F_REQUEST
}

func (message *CreateDevice) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Name})
	return buff.Len()
}

func (message *CreateDevice) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Name})
	return buff.Bytes()
}


type CreateDeviceResult struct {
	Id uint64
}

func DeserializeCreateAccountResult(data []byte) *CreateDeviceResult {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(CreateDeviceResult)
	decoder.Decode(result)
	return result
}

func (this *CreateDeviceResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 5
}
