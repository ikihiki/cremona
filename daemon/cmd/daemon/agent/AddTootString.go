package agent

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type AddTootString struct {
	Uuid [16]byte
	Text string
}

func (mes *AddTootString) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 55
}

func (mes *AddTootString)Deserialize(data []byte) (*AddTootString, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(AddTootString)
	err := decoder.Decode(result)
	return result, err
}

type AddTootStringResult struct {
	Uuid   [16]byte
	Result int
}

func (this *AddTootStringResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 65
}
func (this *AddTootStringResult) GetFlags() uint16 {
	return 0
}

func (message *AddTootStringResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Len()
}

func (message *AddTootStringResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Bytes()
}
