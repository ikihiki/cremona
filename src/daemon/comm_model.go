package main

import (
	"bytes"
	"encoding/gob"
	"errors"
	"fmt"

	"github.com/vmihailenco/msgpack"
)

func GetBytes(key interface{}) ([]byte, error) {
	var buf bytes.Buffer
	enc := gob.NewEncoder(&buf)
	err := enc.Encode(key)
	if err != nil {
		return nil, err
	}
	return buf.Bytes(), nil

}

type CreateAccount struct {
	Name string
}

const CreateAccountType = 0

func genCreateAccount(name string) *CreateAccount {
	return &CreateAccount{
		Name: name,
	}
}

func (message *CreateAccount) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.Encode(message)
	return buff.Len()
}

func (message *CreateAccount) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
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

func DeserializeUuidFromInterface(uuid []interface{}) (*[16]byte, error) {
	result := new([16]byte)
	fmt.Printf("DeserializeUuidFromInterface %#v\n", uuid)
	if len(uuid) != 16 {
		return result, errors.New("bad format")
	}
	fmt.Printf("DeserializeUuidFromInterface %#v\n", uuid)

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
	fmt.Printf("DeserializeUuidFromInterface %#v\n", uuid)

	return result, nil
}

type NewToot struct {
	uuid [16]byte
}

func DeserializeNewToot(data []byte) (*NewToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(NewToot)
	strctInterface, err := decoder.DecodeInterface()
	if err != nil {
		return nil, err
	}
	strct, ok := strctInterface.([]interface{})
	if !ok || len(strct) != 1 {
		return nil, errors.New("bad format")
	}

	uuid, ok := strct[0].([]interface{})
	if !ok {
		return nil, errors.New("bad format")
	}
	uuidArr, err := DeserializeUuidFromInterface(uuid)
	if err != nil {
		return nil, err
	}
	for i := 0; i < 16; i++ {
		result.uuid[i] = uuidArr[i]
	}
	return result, err
}

type NewTootResult struct {
	uuid   [16]byte
	result int
}

func (message *NewTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Len()
}

func (message *NewTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Bytes()
}

type AddTootString struct {
	uuid [16]byte
	text string
}

func DeserializeAddTootString(data []byte) (*AddTootString, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(AddTootString)
	fmt.Printf("Decode add toot Message %#v\n", data)
	strctInterface, err := decoder.DecodeInterface()
	if err != nil {
		return nil, err
	}
	fmt.Printf("1")
	strct, ok := strctInterface.([]interface{})
	if !ok || len(strct) != 2 {
		return nil, errors.New("bad format")
	}
	fmt.Printf("2")
	uuid, ok := strct[0].([]interface{})
	if !ok {
		return nil, errors.New("bad format")
	}
	fmt.Printf("3")
	uuidArr, err := DeserializeUuidFromInterface(uuid)
	if err != nil {
		return nil, err
	}
	for i := 0; i < 16; i++ {
		result.uuid[i] = uuidArr[i]
	}
	fmt.Printf("4")
	text, ok := strct[1].(string)
	if !ok {
		return nil, errors.New("bad format")
	}
	result.text = text
	fmt.Printf("Decode add toot Message %#v\n", result)
	return result, err
}

type AddTootStringResult struct {
	uuid   [16]byte
	result int
}

func (message *AddTootStringResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Len()
}

func (message *AddTootStringResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Bytes()
}

type SendToot struct {
	uuid [16]byte
}

func DeserializeSendToot(data []byte) (*SendToot, error) {
	buff := new(bytes.Buffer)
	buff.Write(data)
	decoder := msgpack.NewDecoder(buff)
	result := new(SendToot)
	strctInterface, err := decoder.DecodeInterface()
	if err != nil {
		return nil, err
	}
	strct, ok := strctInterface.([]interface{})
	if !ok || len(strct) != 1 {
		return nil, errors.New("bad format")
	}

	uuid, ok := strct[0].([]interface{})
	if !ok {
		return nil, errors.New("bad format")
	}
	uuidArr, err := DeserializeUuidFromInterface(uuid)
	if err != nil {
		return nil, err
	}
	for i := 0; i < 16; i++ {
		result.uuid[i] = uuidArr[i]
	}
	return result, err
}

type SendTootResult struct {
	uuid   [16]byte
	result int
}

func (message *SendTootResult) Len() int {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Len()
}

func (message *SendTootResult) Serialize() []byte {
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.UseArrayForStructs(true)
	encoder.UseCompactEncoding(true)
	encoder.EncodeArrayLen(2)
	encoder.EncodeArrayLen(16)
	for i := 0; i < 16; i++ {
		encoder.EncodeUint8(uint8(message.uuid[i]))
	}
	encoder.EncodeInt32(int32(message.result))
	return buff.Bytes()
}
