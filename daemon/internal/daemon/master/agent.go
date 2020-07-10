package master

import (
	"context"
	"io"
	"log"
	"os"
	"os/exec"
	"syscall"

	"github.com/ikihiki/cremona/daemon/internal/daemon"
	"github.com/sourcegraph/jsonrpc2"
)

type readwrite struct {
	read  io.Reader
	write io.Writer
}

func (this *readwrite) Read(p []byte) (n int, err error) {
	return this.read.Read(p)
}

func (this *readwrite) Write(p []byte) (n int, err error) {
	return this.write.Write(p)
}

func (this *readwrite) Close() error {
	return nil
}

type masterHandle struct{}

func (this *masterHandle) Handle(context.Context, *jsonrpc2.Conn, *jsonrpc2.Request) {

}

type agent struct {
	info    user
	logger  *log.Logger
	config  daemon.UserConfigrator
	process *exec.Cmd
	conn    *jsonrpc2.Conn
}

func newAgent(user user, logger *log.Logger, userConfigLoader daemon.UserConfigLoader) (*agent, error) {
	cfg, err := userConfigLoader.LoadConfig(user.HomeDirectory)
	if err != nil {
		logger.Printf("cant load config user name : %s", user.Username)
		return nil, err
	}
	logger.Printf("load config user name : %s", user.Username)
	executable, err := os.Executable()
	if err != nil {
		return nil, err
	}
	cmd := exec.Command(executable, "-agent", "-dir", user.HomeDirectory)
	cmd.SysProcAttr = &syscall.SysProcAttr{
		Cloneflags: syscall.CLONE_NEWUSER,
		UidMappings: []syscall.SysProcIDMap{
			{ContainerID: 0, HostID: int(user.UserID), Size: 1},
		},
	}
	return &agent{info: user, logger: logger,config: cfg, process: cmd}, nil
}

func (agent *agent) run() error {
	log.Println("Run Daemon. ", agent.info.Username)
	err := agent.process.Start()
	if err != nil {
		return err
	}
	
	rwc := readwrite{read: agent.process.Stdin, write: agent.process.Stdout}
	codec := jsonrpc2.VarintObjectCodec{}
	

		stream := jsonrpc2.NewBufferedStream(&rwc, codec)
	handle := masterHandle{}

	agent.conn = jsonrpc2.NewConn(context.Background(), stream, &handle)
	return nil
}

func (agent *agent) getIsRunning() bool {
	return agent.process.ProcessState.Exited()
}

func (agent *agent) getPid() int {
	return agent.process.Process.Pid
}
