package agent

import (
	"context"
	"os"
	"os/signal"

	"github.com/ikihiki/cremona/daemon/internal/config"
	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/log"
	"github.com/ikihiki/cremona/daemon/internal/mastdon"
	"github.com/ikihiki/cremona/daemon/internal/toot"
	"golang.org/x/sync/errgroup"
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
	// シグナル用のチャネル定義
    quit := make(chan os.Signal)

    // 受け取るシグナルを設定
	signal.Notify(quit, os.Interrupt)
	ctx, cancel := context.WithCancel(context.Background())
	eg, ctx := errgroup.WithContext(ctx)
	
	go eg.Go(func() error {return device.RunMessageLoop(ctx)}) 

	<-quit
	cancel()
	err = eg.Wait()	
	if err != nil {
		logger.Printf("error in run device : %v\n", err)
	}
	err = device.DestroyDevice()
	if err != nil {
		logger.Printf("error in destory device: %v\n", err)
	}
	connection.DisConnect()
}