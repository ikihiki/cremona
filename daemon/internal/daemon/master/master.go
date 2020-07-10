package master

import (
	"encoding/json"
	"log"
	"strconv"

	"github.com/ikihiki/cremona/daemon/internal/daemon"
	"github.com/thoas/go-funk"
)

type userID int

type user struct {
	HomeDirectory string
	Username      string
	UserID        userID
	
}

type Master struct {
	logger *log.Logger
	agents []*agent
	
}


func NewMaster(logger *log.Logger, config daemon.GlobalConfigrator, userConfigLoader daemon.UserConfigLoader) Master {
	logger.Println("new master")
	users := searchHomeDir()
	logger.Printf("find %d users\n", len(users))
	agents := funk.Map(users, func(user user) *agent{
		a, err := newAgent(user, logger, userConfigLoader)
		if err !=nil{
			return nil
		}
		return a
	})
	agents = funk.Filter(agents, func(agent *agent) bool{return agent != nil})

	return Master{
		logger: logger,
		agents: agents.([]*agent),
	}
}

func DumpJosn(master *Master) (string, error) {
	maps := funk.Map(master.agents, func(agent agent)map[string]string{
		return map[string]string{
			"HomeDirectory": agent.info.HomeDirectory,
			"Username": agent.info.Username,
			"UserID": strconv.Itoa(int(agent.info.UserID)),
		}
	})

	b, e:= json.MarshalIndent(maps, "", "\t")
	return string(b),e
}
