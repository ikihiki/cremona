package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type AddTootText struct {
	TootId   uint32
	DeviceId uint32
	Text     string
}

func (mes *AddTootText) GetMessageTypeId() uint16 {
	return GetAddTootTextMessageTypeId()
}

func GetAddTootTextMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 10
}

func DeserializeAddTootText(data []byte) (*AddTootText, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(AddTootText)
	err := decoder.Decode(result)
	return result, err
}

type AddTootTextResult struct {
	TootId   uint32
	DeviceId uint32
	Result   int
}

func (this *AddTootTextResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 11
}
func (this *AddTootTextResult) GetFlags() uint16 {
	return 0
}

func (message *AddTootTextResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Len()
}

func (message *AddTootTextResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Bytes()
}
