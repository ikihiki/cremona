package main

import (
	"flag"
	"fmt"
	"os"
	"time"

	"github.com/ikihiki/cremona/daemon/internal/driver"
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

func main() {
	port := flag.Int("port", 17, "port")
	name := flag.String("name", "test_device", "name of divice")
	wait := flag.Uint("wait", 120, "wait sec for destory device ")
	flag.Parse()

	config := &Config{name: *name}

	connection := driver.NewConnection(*port)
	err := connection.Connect()
	if err != nil {
		panic(err)
	}

	device, err := driver.NewDevice(connection, config, nil)
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
    time.Sleep(time.Second * time.Duration(*wait))

	err = device.DestroyDevice()
		if err != nil {
		panic(err)
	}

	_, err = os.Stat("/dev/crmna_test_device")
	fmt.Printf("%#v\n", err)
}
