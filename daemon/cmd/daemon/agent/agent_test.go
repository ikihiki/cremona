package agent

import (
	"bytes"
	"log"
	"reflect"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/mock"
	"github.com/vmihailenco/msgpack"
)

type MockConfig struct {
	mock.Mock
}

func (mock *MockConfig) GetConfig() *Config {
	result := mock.Called()
	return result.Get(0).(*Config)
}

type MockDriver struct {
	mock.Mock
}

func (mock *MockDriver) Connect() error {
	result := mock.Called()
	return result.Error(0)
}

func (mock *MockDriver) DisConnect() {
	mock.Called()
}
func (mock *MockDriver) GetIsConnectiong() bool {
	result := mock.Called()
	return result.Bool(0)
}
func (mock *MockDriver) SendMessage(message Message) error {
	log.Printf("%#v", message.Serialize())
	result := mock.Called(message)
	return result.Error(0)
}
func (mock *MockDriver) ReciveMessage() ([]RecivedMessage, error) {
	result := mock.Called()
	return result.Get(0).([]RecivedMessage), result.Error(1)
}

type MockMastdon struct {
	mock.Mock
}

func (mock *MockMastdon) Toot(toot *Toot) {
	mock.Called(toot)

}

func TestAgent_Run_ReciveNewToot(t *testing.T) {
	config := &Config{}
	configLoader := new(MockConfig)
	configLoader.On("GetConfig").Return(config)
	mastdon := new(MockMastdon)

	uuid := [16]byte{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 223}
	message := &NewToot{Uuid: uuid}
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.Uuid})
	messageData := buff.Bytes()
	log.Printf("%#v\n", messageData)
	driver := new(MockDriver)
	driver.On("Connect").Return(nil)
	driver.On("GetIsConnectiong").Return(true).Once()
	driver.On("GetIsConnectiong").Return(false)
	driver.On("ReciveMessage").Return([]RecivedMessage{RecivedMessage{Type: message.GetMessageTypeId(), Data: messageData}}, nil)
	driver.On("ReciveMessage").Return()
	driver.On("SendMessage", mock.AnythingOfType("*agent.NewTootResult")).Return(nil)

	target, err := NewAgent(configLoader, mastdon, driver)
	assert.Nil(t, err)

	err = target.Run()
	assert.Nil(t, err)
	driver.AssertCalled(t, "SendMessage", mock.MatchedBy(func(message Message) bool {
		result := message.(*NewTootResult)
		return reflect.DeepEqual(result.Uuid, uuid) && result.Result == 0
	}))
}

func TestAgent_Run_ReciveAddTootString(t *testing.T) {
	config := &Config{}
	configLoader := new(MockConfig)
	configLoader.On("GetConfig").Return(config)
	mastdon := new(MockMastdon)

	uuid := [16]byte{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 223}
	text := "text"
	message := &AddTootString{Uuid: uuid, Text: text}
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.Uuid, message.Text})
	messageData := buff.Bytes()
	//log.Printf("%#v\n", messageData)
	driver := new(MockDriver)
	driver.On("Connect").Return(nil)
	driver.On("GetIsConnectiong").Return(true).Once()
	driver.On("GetIsConnectiong").Return(false)
	driver.On("ReciveMessage").Return([]RecivedMessage{RecivedMessage{Type: message.GetMessageTypeId(), Data: messageData}}, nil)
	driver.On("ReciveMessage").Return()
	driver.On("SendMessage", mock.AnythingOfType("*agent.AddTootStringResult")).Return(nil)

	target, err := NewAgent(configLoader, mastdon, driver)
	assert.Nil(t, err)

	target.toots[uuid] = &Toot{}

	err = target.Run()
	assert.Nil(t, err)
	driver.AssertCalled(t, "SendMessage", mock.MatchedBy(func(message Message) bool {
		result := message.(*AddTootStringResult)
		return reflect.DeepEqual(result.Uuid, uuid) && result.Result == len(text)
	}))
	assert.Fail(t, "test")
}

func TestAgent_Run_ReciveSendToot (t *testing.T) {
	config := &Config{}
	configLoader := new(MockConfig)
	configLoader.On("GetConfig").Return(config)

	mastdon := new(MockMastdon)
	mastdon.On("Toot",mock.AnythingOfType("*agent.Toot")).Return()

	uuid := [16]byte{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 223}
	text := "text"
	message := &SendToot{Uuid: uuid}
	buff := new(bytes.Buffer)
	encoder := msgpack.NewEncoder(buff)
	encoder.Encode([]interface{}{message.Uuid})
	messageData := buff.Bytes()
	log.Printf("%#v\n", messageData)
	driver := new(MockDriver)
	driver.On("Connect").Return(nil)
	driver.On("GetIsConnectiong").Return(true).Once()
	driver.On("GetIsConnectiong").Return(false)
	driver.On("ReciveMessage").Return([]RecivedMessage{RecivedMessage{Type: message.GetMessageTypeId(), Data: messageData}}, nil)
	driver.On("ReciveMessage").Return()
	driver.On("SendMessage", mock.AnythingOfType("*agent.SendTootResult")).Return(nil)

	target, err := NewAgent(configLoader, mastdon, driver)
	assert.Nil(t, err)

	target.toots[uuid] = &Toot{}
	target.toots[uuid].AddStatusText(text)

	err = target.Run()
	assert.Nil(t, err)

	mastdon.AssertCalled(t, "Toot", mock.MatchedBy(func(toot *Toot) bool {
		return toot.builder.String() == text
	}))

	driver.AssertCalled(t, "SendMessage", mock.MatchedBy(func(message Message) bool {
		result := message.(*SendTootResult)
		return reflect.DeepEqual(result.Uuid, uuid) && result.Result == 0
	}))
}