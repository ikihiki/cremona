package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type AddTootElement struct {
	TootId   uint32
	DeviceId uint32
	ElementId uint32
	Index uint32
	Text     string
}

func (mes *AddTootElement) GetMessageTypeId() uint16 {
	return GetAddTootTextMessageTypeId()
}

func GetAddTootTextMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 10
}

func DeserializeAddTootElement(data []byte) (*AddTootElement, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(AddTootElement)
	err := decoder.Decode(result)
	return result, err
}

type AddTootElementResult struct {
	TootId   uint32
	DeviceId uint32
	ElementId uint32
	Result   int
}

func (this *AddTootElementResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 11
}
func (this *AddTootElementResult) GetFlags() uint16 {
	return 0
}

func (message *AddTootElementResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.ElementId, message.Result})
	return buff.Len()
}

func (message *AddTootElementResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.ElementId, message.Result})
	return buff.Bytes()
}
