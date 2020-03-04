package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type DestroyDevice struct{
	Id uint64
}

func (this *DestroyDevice) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 6
}
func (this *DestroyDevice) GetFlags() uint16 {
		return unix.NLM_F_REQUEST
}

func (message *DestroyDevice) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Id})
	return buff.Len()
}

func (message *DestroyDevice) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Id})
	return buff.Bytes()
}


type DestroyDeviceResult struct {
	Id uint64
}

func DeserializeDestroyDeviceResult(data []byte) (*DestroyDeviceResult, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(DestroyDeviceResult)
	err := decoder.Decode(result)
	return result, err
}

func (this *DestroyDeviceResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 7
}
