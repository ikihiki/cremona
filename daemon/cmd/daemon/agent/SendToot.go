package agent

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type SendToot struct {
	Uuid [16]byte
}

func (mes *SendToot) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 75
}

func (mes *SendToot)Deserialize(data []byte) (*SendToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(SendToot)
	err := decoder.Decode(result)
	return result, err
}

type SendTootResult struct {
	Uuid   [16]byte
	Result int
}

func (this *SendTootResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 85
}
func (this *SendTootResult) GetFlags() uint16 {
	return 0
}

func (message *SendTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Len()
}

func (message *SendTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Bytes()
}
