package message

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type SendToot struct {
	TootId   uint64
	DeviceId uint64
}

func (mes *SendToot) GetMessageTypeId() uint16 {
	return GetSendTootMessageTypeId()
}

func GetSendTootMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 12
}

func DeserializeSendToot(data []byte) (*SendToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(SendToot)
	err := decoder.Decode(result)
	return result, err
}

type SendTootResult struct {
	TootId   uint64
	DeviceId uint64
	Result   int
}

func (this *SendTootResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE * 13
}
func (this *SendTootResult) GetFlags() uint16 {
	return 0
}

func (message *SendTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Len()
}

func (message *SendTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.TootId, message.DeviceId, message.Result})
	return buff.Bytes()
}
