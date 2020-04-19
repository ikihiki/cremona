package main

import (
	"flag"
	"fmt"
	"os"
	"sync"
	"time"

	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/toot"
)

type Config struct {
	name string
}

func (config *Config) GetDeviceName() string {
	return config.name
}
func (this *Config) GetUserId() uint32 {
	return uint32(os.Getuid())
}

type TootSend struct{}

func (this *TootSend) SendToot(toot *toot.Toot) error {
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

	stats, _ := driver.GetDriverStats(connection)
	fmt.Printf("%#v\n", stats)

	file, err := os.Stat("/dev/crmna_test_device")
	if err != nil {
		panic(err)
	}
	fmt.Printf("%#v\n", file)

	fmt.Printf("%#v\n", device)

	wg := &sync.WaitGroup{}
	cancel := make(chan interface{})
	go device.RunMessageLoop(cancel, wg)

	time.Sleep(time.Second)

	fh, err := os.OpenFile("/dev/crmna_test_device", os.O_RDWR, 0777)
	if err != nil {
		panic(err)
	}
	println("opend")

	count, err := fh.WriteString("test_txt")
	if err != nil {
		panic(err)
	}
	fmt.Printf("write count %d", count)

	err = fh.Close()
	if err != nil {
		panic(err)
	}
	cancel <- true
	close(cancel)

	err = device.DestroyDevice()
	if err != nil {
		panic(err)
	}

	_, err = os.Stat("/dev/crmna_test_device")
	fmt.Printf("%#v\n", err)
}
