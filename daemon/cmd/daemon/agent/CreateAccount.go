package agent

import (
	"bytes"
	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type CreateAccount struct {
	Name string
}

func (this *CreateAccount) GetMessageTypeId() uint16 {
	return 0
}
func (this *CreateAccount) GetFlags() uint16 {
	return unix.NLM_F_ACK
}

func (message *CreateAccount) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode(message)
	return buff.Len()
}

func (message *CreateAccount) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode(message)
	return buff.Bytes()
}


type CreateAccountResult struct {
	Result int
	Name   string
}

func DeserializeCreateAccountResult(data []byte) *CreateAccountResult {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(CreateAccountResult)
	decoder.Decode(result)
	return result
}

func (this *CreateAccountResult) GetMessageTypeId() uint16 {
	return 1
}