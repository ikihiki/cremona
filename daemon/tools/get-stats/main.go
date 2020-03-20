package main

import (
	"flag"
	"fmt"

	"github.com/ikihiki/cremona/daemon/internal/driver"
)

func main() {
	port := flag.Int("port", 17, "port")
	flag.Parse()

	connection := driver.NewConnection(*port)
	err := connection.Connect()
	if err != nil {
		panic(err)
	}
	stats, err := driver.GetDriverStats(connection)
	if err != nil {
		panic(err)
	}

	fmt.Printf("%#v\n", stats)
}
