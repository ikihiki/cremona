package agent

import "errors"

type Message interface {
	GetMessageTypeId() uint16
	GetFlags() uint16
	Len() int
	Serialize() []byte
}

func DeserializeUuidFromInterface(uuid []interface{}) (*[16]byte, error) {
	result := new([16]byte)
	if len(uuid) != 16 {
		return result, errors.New("bad format")
	}

	for i := 0; i < 16; i++ {
		u, ok := uuid[i].(uint8)
		if ok {
			result[i] = byte(u)
		} else {
			i8, ok := uuid[i].(int8)
			if !ok {
				return result, errors.New("bad format")
			}
			result[i] = byte(i8)
		}
	}

	return result, nil
}
