package agent

import (
	"os"
	"os/signal"
	"sync"

	"github.com/ikihiki/cremona/daemon/internal/config"
	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/log"
	"github.com/ikihiki/cremona/daemon/internal/mastdon"
	"github.com/ikihiki/cremona/daemon/internal/toot"
)


func StartAgent(baseDir string) {
	logger := log.NewStdLogger("", true)
	config, err := config.LoadConfig(baseDir)
	if err != nil {
		logger.Fatalf("cannot load config: %v\n", err)
	}

	mastdonClient, err := mastdon.NewMastdonCilent(config)
	if err != nil {
		logger.Fatalf("cannot create mastdon client: %v\n", err)
	}

	tootManage, err := toot.NewTootManage(config, mastdonClient)
	if err != nil {
		logger.Fatalf("cannot create toot manager: %v\n", err)
	}

	connection := driver.NewConnection(17)
	err = connection.Connect()
	if err != nil {
		logger.Fatalf("cannot connect kernel module: %v\n", err)
	}

	device, err := driver.NewDevice(connection, config, tootManage)
	if err != nil {
		connection.DisConnect()
		logger.Fatalf("cannot create device: %v\n", err)
	}

	wg := &sync.WaitGroup{}
	cancel := make(chan interface{})
    // シグナル用のチャネル定義
    quit := make(chan os.Signal)

    // 受け取るシグナルを設定
    signal.Notify(quit, os.Interrupt)

	go device.RunMessageLoop(cancel, wg)

	<-quit
	cancel <- nil
	wg.Wait()
	err = device.DestroyDevice()
	connection.DisConnect()
}