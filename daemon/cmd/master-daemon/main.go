package main

import (
	"fmt"

	"github.com/ikihiki/cremona/daemon/internal/user"
)

func main() {
	users, err := user.GetUidsHaveConfig(".cremona")
	if err != nil {
		panic(err)
	}
	fmt.Printf("#%v", users)
}
