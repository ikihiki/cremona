package master

import (
	"bufio"
	"errors"
	"log"
	"os"
	"os/exec"
	"os/user"
	"time"

	"github.com/google/uuid"
)

type Agent struct {
	user    user.User
	process *exec.Cmd
	healthCheckRound uint64
	errorCount int
}

func NewAgent(user user.User) (*Agent, error) {
	executable, err := os.Executable()
	if err != nil {
		return nil, err
	}
	cmd := exec.Command(executable, "-agent", "-uid", user.Uid)
	return &Agent{user: user, process: cmd, healthCheckRound: 0}, nil
}

func (agent *Agent) Run(cancel chan string) error {
	log.Println("Run Daemon. ", agent.user.Username)
	err := agent.process.Start()
	if err != nil {
		return err
	}

	started, err := agent.waitStdIn(10, cancel)
	if err != nil {
		if err.Error() == "Timeout"
	}
}

func (agent *Agent) GetIsRunning() bool {
	return agent.process.ProcessState.Exited()
}

func(agent *Agent)GetPid () int {
	return agent.process.Process.Pid
}

func (agent *Agent)readStdIn(out chan string) error {
	scanner := bufio.NewScanner(agent.process.Stdin)
	for scanner.Scan() {
		out <- scanner.Text()
	}
	return scanner.Err()	
}

func (agent *Agent)waitStdIn(timeout int, cancel chan string) (string, error) {
	if !agent.GetIsRunning() {
		return "", errors.New("Process not running")
	}

	out := make(chan string, 1)
	go agent.readStdIn(out)

	select {
	case result := <- out:
		return result, nil
	case <-time.After(time.Duration(timeout) * time.Millisecond):
		return "", errors.New("Timeout")
	case <-cancel:
		return "", errors.New("Cancel")
	}
}

func  (agent *Agent)writeLineStdout(text string) error{
	if !agent.GetIsRunning() {
		return errors.New("Process not running")
	}

	writer := bufio.NewWriter(agent.process.Stdout)
	_, err := writer.WriteString(text)
	if err != nil {
		return  err
	}
	_, err = writer.WriteString("\n")
	if err != nil {
		return  err
	}
	return nil
}