package main

import (
	"context"
	"flag"
	"fmt"
	"os"
	"time"

	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/toot"
	"golang.org/x/sync/errgroup"
)

type Config struct{
	name string
}

func (config *Config) GetDeviceName() string{
	return config.name
}
func (this *Config) GetUserId() uint32 {
	return uint32(os.Getuid())
}

type TootSend struct{}

func (this *TootSend) SendToot(toot *toot.Toot) error {
	fmt.Printf("SendToot %#v\n", toot)
	fmt.Printf("SendToot %s\n", toot.GetText())
	return nil
}

func main() {
	port := flag.Int("port", 17, "port")
	name := flag.String("name", "test_device", "name of divice")
	flag.Parse()


	config := &Config{name: *name}

	connection := driver.NewConnection(*port)
	err := connection.Connect()
	if err != nil {
		panic(err)
	}

	tootManage, err := toot.NewTootManage(config, &TootSend{})
	if err != nil {
		panic(err)
	}

	device, err := driver.NewDevice(connection, config, tootManage)
	if err != nil {
		panic(err)
	}

	stats, _ := driver.GetDeviceHealth(connection, device.Id)
	fmt.Printf("%#v\n", stats)

	file, err := os.Stat("/dev/crmna_test_device")
	if err != nil {
		panic(err)
	}
	fmt.Printf("%#v\n", file)

	fmt.Printf("%#v\n", device)

	ctx, cancel := context.WithCancel(context.Background())
	eg, ctx := errgroup.WithContext(ctx)
	go eg.Go(func() error {return device.RunMessageLoop(ctx)}) 

	time.Sleep(time.Second)

	fh, err := os.Open("/dev/crmna_test_device")
	if err != nil {
		panic(err)
	}
	println("opend")
	err = fh.Close()
	if err != nil {
		panic(err)
	}
	cancel()
	err = eg.Wait()
	if err != nil {
		panic(err)
	}


	err = device.DestroyDevice()
	if err != nil {
		panic(err)
	}

	_, err = os.Stat("/dev/crmna_test_device")
	fmt.Printf("%#v\n", err)
}
