package agent

import (
	"bytes"

	"github.com/vmihailenco/msgpack"
	"golang.org/x/sys/unix"
)

type NewToot struct {
	Uuid [16]byte
}

func (mes *NewToot) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 35
}

func (mes *NewToot)Deserialize(data []byte) (*NewToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(NewToot)
	err := decoder.Decode(result)
	// if err != nil {
	// 	return nil, err
	// }
	// strct, ok := strctInterface.([]interface{})
	// if !ok || len(strct) != 1 {
	// 	return nil, errors.New("bad format")
	// }

	// uuid, ok := strct[0].([]interface{})
	// if !ok {
	// 	return nil, errors.New("bad format")
	// }
	// uuidArr, err := DeserializeUuidFromInterface(uuid)
	// if err != nil {
	// 	return nil, err
	// }
	// for i := 0; i < 16; i++ {
	// 	result.Uuid[i] = uuidArr[i]
	// }
	return result, err
}

type NewTootResult struct {
	Uuid   [16]byte
	Result int
}

func (this *NewTootResult) GetMessageTypeId() uint16 {
	return unix.NLMSG_MIN_TYPE + 45
}
func (this *NewTootResult) GetFlags() uint16 {
	return 0
}

func (message *NewTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Len()
}

func (message *NewTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseCompactEncoding(true)
	encoder.Encode([]interface{}{message.Uuid, message.Result})
	return buff.Bytes()
}
