package master

import (
	"context"
	"io"
	"log"
	"os"
	"os/exec"
	"syscall"

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

func (this *masterHandle)Handle(context.Context, *jsonrpc2.Conn, *jsonrpc2.Request){
	
}

type Agent struct {
	info    User
	process *exec.Cmd
	conn    *jsonrpc2.Conn
}

func NewAgent(user User) (*Agent, error) {
	executable, err := os.Executable()
	if err != nil {
		return nil, err
	}
	cmd := exec.Command(executable, "-agent", "-dir", user.HomeDirectory)
	cmd.SysProcAttr = &syscall.SysProcAttr{
		Cloneflags: syscall.CLONE_NEWUSER,
		UidMappings: []syscall.SysProcIDMap{
			{ContainerID: 0, HostID: int(user.UserId), Size: 1},
		},
	}
	rwc := readwrite{read: cmd.Stdin, write: cmd.Stdout}
	codec:=jsonrpc2.VarintObjectCodec{}
	stream := jsonrpc2.NewBufferedStream(&rwc, codec)
	handle:=masterHandle{}
	conn:= jsonrpc2.NewConn(context.Background(), stream, &handle)
	return &Agent{info: user, process: cmd, conn: conn}, nil
}

func (agent *Agent) Run() error {
	log.Println("Run Daemon. ", agent.info.Username)
	err := agent.process.Start()
	if err != nil {
		return err
	}
	return nil
}

func (agent *Agent) GetIsRunning() bool {
	return agent.process.ProcessState.Exited()
}

func (agent *Agent) GetPid() int {
	return agent.process.Process.Pid
}
