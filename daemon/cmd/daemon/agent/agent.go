package agent

import (
	"log"
	"github.com/google/uuid"
)

type Agent struct {
	config  ConfigLoader
	mastdon MastdonClient
	driver  DriverCommunicator
	toots    map[uuid.UUID]*Toot
}

type ConfigLoader interface {
	GetConfig() *Config
}

type MastdonClient interface {
	Toot(toot *Toot)
}

func NewAgent(config ConfigLoader, mstdon MastdonClient, driver DriverCommunicator) (*Agent, error) {
	agent := Agent{config: config, mastdon: mstdon, driver: driver, toots: make(map[uuid.UUID]*Toot)}
	return &agent, nil
}

func (agent *Agent) Run() error {
	err := agent.driver.Connect()
	if err != nil {
		return err
	}

	for agent.driver.GetIsConnectiong() {
		mesgs, err := agent.driver.ReciveMessage()
		if err != nil {
			log.Println("error in runnning recive message", err)
		}
		agent.analyzeMessage(mesgs)
	}
	return nil
}

func (agent *Agent) analyzeMessage(messages []RecivedMessage) {
	for _, message := range messages {
		var err error
		switch message.Type {
		case (&NewToot{}).GetMessageTypeId():
			err = agent.processNewToot(message.Data)
		case (&AddTootString{}).GetMessageTypeId():
			err = agent.processAddTootString(message.Data)
		case (&SendToot{}).GetMessageTypeId():
			err = agent.processSendToot(message.Data)
		}
		if err != nil {
			log.Println(err, message)
		}
	}
}

func (agent *Agent) processNewToot(data []byte) error {
	newToot, err := (&NewToot{}).Deserialize(data)
	if err != nil {
		return err
	}
	agent.toots[newToot.Uuid] = &Toot{}
	result := &NewTootResult{Uuid: newToot.Uuid, Result: 0}
	agent.driver.SendMessage(result)
	return nil
}


func (agent *Agent)processAddTootString(data []byte) error {
	addTootString, err := (&AddTootString{}).Deserialize(data)
	if err != nil {
		return err
	}
	agent.toots[addTootString.Uuid].AddStatusText(addTootString.Text)
	result := &AddTootStringResult{Uuid: addTootString.Uuid, Result: len(addTootString.Text)}
	agent.driver.SendMessage(result)
	return nil
}

func (agent *Agent)processSendToot(data []byte) error {
	addTootString, err := (&SendToot{}).Deserialize(data)
	if err != nil {
		return err
	}
	agent.mastdon.Toot(agent.toots[addTootString.Uuid])
	result := &SendTootResult{Uuid: addTootString.Uuid, Result: 0}
	agent.driver.SendMessage(result)
	return nil
}