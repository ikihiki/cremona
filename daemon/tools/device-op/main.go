package main

import (
	"flag"
	"fmt"
	"os"

	"github.com/ikihiki/cremona/daemon/internal/driver"
)

func main() {
	port := flag.Int("port", 17, "port")
	name := flag.String("name", "test_device", "name of divice")
	flag.Parse()

	driver, err := driver.NewDriver(*port, nil)
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
	err = device.DestroyDevice()
		if err != nil {
		panic(err)
	}

	_, err = os.Stat("/dev/crmna_test_device")
	fmt.Printf("%#v\n", err)
}
