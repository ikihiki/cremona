package master

import (
	"context"
	"log"
	"net"
	"os"

	"github.com/ikihiki/cremona/daemon/generated/proto"
	"github.com/ikihiki/cremona/daemon/internal/user"
	"google.golang.org/grpc"
)

type Master struct {
	agents map[string]*Agent
}

func NewMaster() *Master {
	master := Master{agents: make(map[string]*Agent)}
	return &master
}

func (master *Master) Run() error {
	users, err := user.GetUidsHaveConfig(".cremona")
	if err != nil {
		return err
	}

	log.Printf("%#v\n", users)
	for _, user := range users {
		agent, err := NewAgent(user)
		master.agents[user.Uid] = agent
		if err != nil {
			log.Fatalln("Cannot create daemon process", err)
		}

		err = agent.Run()
		if err != nil {
			log.Fatalln("Cannot run daemon process", err)
		}
	}
	master.listen()
}


func (master *Master) listen() error {
	file := "/tmp/uds-sample-server.sock"
    // 削除はdeferで予約しておく
    // シグナルハンドラ定義しないと終了時に呼ばれない？
    defer os.Remove(file)
    listener, err := net.Listen("unix", file)
    if err != nil {
        return err
    }
    server := grpc.NewServer()
    catService := &AgentServer{}
	// 実行したい実処理をseverに登録する
    cremona.RegisterAgentServer(server, catService)
    server.Serve(listener)
	return nil
}

type AgentServer struct {

}
func (this *AgentServer) Started(context.Context, *cremona.AgentIdentity) (*cremona.Empty, error){

}
func (this *AgentServer) Error(context.Context, *cremona.AgentErrorMessage) (*cremona.Empty, error){

}
func (this *AgentServer) HealthCheck(cremona.Agent_HealthCheckServer) error{

}

