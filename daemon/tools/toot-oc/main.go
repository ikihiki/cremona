package main

import (
	"flag"
	"fmt"
	"os"
	"sync"
	"time"

	"github.com/ikihiki/cremona/daemon/internal/driver"
	"github.com/ikihiki/cremona/daemon/internal/message"
)

type cb struct {
}

func (cb cb) NewToot(msg *message.NewToot) *message.NewTootResult {
	return &message.NewTootResult{TootId: msg.TootId, DeviceId: msg.DeviceId, Result: 0}
}

func (cb cb) SendToot(msg *message.SendToot) *message.SendTootResult {
	return &message.SendTootResult{TootId: msg.TootId, DeviceId: msg.DeviceId, Result: 0}
}

func main() {
	port := flag.Int("port", 17, "port")
	name := flag.String("name", "test_device", "name of divice")
	flag.Parse()

	driver, err := driver.NewDriver(*port, &cb{})
	if err != nil {
		panic(err)
	}
	device, err := driver.CreateDevice(*name)
	if err != nil {
		panic(err)
	}

	stats, _ := driver.GetStats()
	fmt.Printf("%#v\n", stats)

	file, err := os.Stat("/dev/crmna_test_device")
	if err != nil {
		panic(err)
	}
	fmt.Printf("%#v\n", file)

	fmt.Printf("%#v\n", device)

	wg := &sync.WaitGroup{}
	cancel := make(chan interface{})
	go process_message(device, cancel, wg)

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
	cancel <- true
	close(cancel)


	err = device.DestroyDevice()
	if err != nil {
		panic(err)
	}

	_, err = os.Stat("/dev/crmna_test_device")
	fmt.Printf("%#v\n", err)
}

func process_message(device *driver.Device, cancel <-chan interface{}, wg *sync.WaitGroup) {
	for {
		select {
		case <-cancel:
			break
		default:
			err := device.ProcessMessages()
			if err != nil {
				println(err)
				break
			}
		}
	}
	wg.Done()
}
