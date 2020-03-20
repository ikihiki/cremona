package main

import (
	"flag"

	"github.com/ikihiki/cremona/daemon/cmd/daemon/agent"
	"github.com/ikihiki/cremona/daemon/cmd/daemon/master"
)

func main() {
	println()
	agentMode := flag.Bool("agent", false, "is agent mode")
	homeDir := flag.String("dir", "", "directory for .cremona")
	flag.Parse()
	if *agentMode {
		agent, err := agent.NewAgent(*homeDir)
		if err != nil {
			panic(err)
		}
		agent.Run()
	} else {
		master := master.NewMaster()
		master.Run()
	}
}
