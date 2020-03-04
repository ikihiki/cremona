package main

import (
	"flag"
	"fmt"

	"github.com/ikihiki/cremona/daemon/internal/driver"
)

func main() {
	port := flag.Int("port", 17, "port")
	flag.Parse()

	driver, err := driver.NewDriver(*port, nil)
	if err != nil {
		panic(err)
	}
	stats, err := driver.GetStats()
	if err != nil {
		panic(err)
	}

	fmt.Printf("%#v\n", stats)
}
