package agent

import (
	"os"
	"os/signal"
	"sync"

	"github.com/ikihiki/cremona/daemon/internal/config"
	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/mastdon"
	"github.com/ikihiki/cremona/daemon/internal/toot"
)

type Agent struct {
	config        *config.Config
	mastdonClient *mastdon.Client
	tootManage    *toot.TootManage
	connection    *driver.Connection
	device        *driver.Device
}

func NewAgent(baseDir string) (*Agent, error) {
	config, err := config.LoadConfig(baseDir)
	if err != nil {
		return nil, err
	}

	mastdonClient, err := mastdon.NewMastdonCilent(config)
	if err != nil {
		return nil, err
	}

	tootManage, err := toot.NewTootManage(config, mastdonClient)
	if err != nil {
		return nil, err
	}

	connection := driver.NewConnection(17)
	err = connection.Connect()
	if err != nil {
		return nil, err
	}

	device, err := driver.NewDevice(connection, config, tootManage)
	if err != nil {
		connection.DisConnect()
		return nil, err
	}

	agent := Agent{
		config:        config,
		mastdonClient: mastdonClient,
		tootManage:    tootManage,
		connection:    connection,
		device:        device,
	}
	return &agent, nil
}

func (agent *Agent) Run() error {
	wg := &sync.WaitGroup{}
	cancel := make(chan interface{})
    // シグナル用のチャネル定義
    quit := make(chan os.Signal)

    // 受け取るシグナルを設定
    signal.Notify(quit, os.Interrupt)

	go agent.device.RunMessageLoop(cancel, wg)

	<-quit
	cancel <- nil
	wg.Wait()
	err := agent.device.DestroyDevice()
	agent.connection.DisConnect()
	return err
}
