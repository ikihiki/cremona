package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type NewToot struct {
	TootId   uint64
	DeviceId uint64
}

func (mes *NewToot) GetMessageTypeId() uint16 {
	return GetNewTootMessageTypeId()
}
func GetNewTootMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 8
}

func DeserializeNewToot(data []byte) (*NewToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(NewToot)
	err := decoder.Decode(result)
	return result, err
}

type NewTootResult struct {
	TootId   uint64
	DeviceId uint64
	Result   int
}

func (this *NewTootResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 9
}
func (this *NewTootResult) GetFlags() uint16 {
	return 0
}

func (message *NewTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Len()
}

func (message *NewTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Bytes()
}
