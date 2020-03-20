package master

import (
	"log"
	"time"
)

type Master struct {
	agents map[UserId]*Agent
}

func NewMaster() *Master {
	master := Master{agents: make(map[UserId]*Agent)}
	return &master
}

func (master *Master) Run() error {
	users := searchHomeDir()
	log.Printf("%#v\n", users)
	for _, user := range users {
		agent, err := NewAgent(user)
		master.agents[user.UserId] = agent
		if err != nil {
			log.Fatalln("Cannot create daemon process", err)
		}

		err = agent.Run()
		if err != nil {
			log.Fatalln("Cannot run daemon process", err)
		}
	}
	for {
		count := 0
		for _, agent := range master.agents {
			if agent.GetIsRunning() {
				count++
			}
		}
		if count == 0 {
			return nil
		}
		time.Sleep(1 * time.Second)
	}

}
