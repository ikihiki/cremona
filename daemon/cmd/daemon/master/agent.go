package master

import (
	"log"
	"os"
	"os/exec"
	"syscall"
)

type Agent struct {
	info    User
	process *exec.Cmd
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
	return &Agent{info: user, process: cmd}, nil
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

func(agent *Agent)GetPid () int {
	return agent.process.Process.Pid
}