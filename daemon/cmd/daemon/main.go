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
	sameUser := flag.Bool("same_user", false, "run agent same user as master")

	flag.Parse()
	if *agentMode {
		agent.StartAgent(*homeDir)
	} else {
		master.StartMaster(*sameUser)
	}
}
