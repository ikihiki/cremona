package master

import (
	"context"
	"fmt"
	"os"
	"os/exec"
	"strings"
	"sync"
	"syscall"

	"github.com/ikihiki/cremona/daemon/internal/log"
)

func StartMaster(sameUser bool) {
	users := searchHomeDir()
	c := context.Background()
	wg := &sync.WaitGroup{}
	for _, user := range users {
		wg.Add(1)
		go startAgent(c, wg, user, sameUser)
	}
	wg.Wait()
}

func startAgent(c context.Context, wg *sync.WaitGroup, user user, sameUser bool) {
	defer wg.Done()
	mlogger := log.NewStdLogger(fmt.Sprintf("[%-20s][master]", user.Username), true)
	alogger := log.NewStdLogger(fmt.Sprintf("[%-20s][agent ]", user.Username), false)
	executable, err := os.Executable()
	if err != nil {
		mlogger.Errorf("error: %v\n", err)
		return
	}

	for i := 0; i < 10; i++ {
		if i > 0 {
			mlogger.Errorf("fail run agent %d times and restart\n", i)
		}

		cmd := exec.Command(executable, "-agent", "-dir", user.HomeDirectory)
		if !sameUser {
			cmd.SysProcAttr = &syscall.SysProcAttr{
				Credential: &syscall.Credential{Uid: user.UserID, Gid: user.GroupID},
			}
		}
		cmd.Stdout = &logWriter{buffer: strings.Builder{}, logger: alogger, isLogError: false}
		cmd.Stderr = &logWriter{buffer: strings.Builder{}, logger: alogger, isLogError: true}
		mlogger.Printf("Run agent. \n")
		err = cmd.Start()
		if err != nil {
			mlogger.Errorf("cannot run agent: %v\n", err)
			continue
		}
		done := make(chan error)
		go func() { done <- cmd.Wait() }()
		select {
		case err = <-done:
			if exiterr, ok := err.(*exec.ExitError); ok {
				if status, ok := exiterr.Sys().(syscall.WaitStatus); ok {
					mlogger.Printf("agent exit Status: %d\n", status.ExitStatus())
					if status.ExitStatus() == -1 {
						break
					}
				}
			} else if err == nil {
				mlogger.Errorf("agent stop normally but it is unexpected\n")
			} else {
				mlogger.Errorf("cmd.Wait: %v\n", err)
			}
		case <-c.Done():
			cmd.Process.Kill()
			return
		}

	}
	mlogger.Errorf("cannt run agent: %v\n", err)
}
