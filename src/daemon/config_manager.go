package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func searchHomeDir() []UserInfo {
	fp, err := os.Open("/etc/passwd")
	if err != nil {
		panic(err)
	}

	var result []UserInfo
	scanner := bufio.NewScanner(fp)
	for scanner.Scan() {
		fields := strings.Split(scanner.Text(), ":")
		fmt.Printf("%s", fields[5])
		user := UserInfo{
			HomeDirectory: fields[5],
			LinuxUserId:   fields[2],
			LinuxUsername: fields[0],
		}

		if isExist(user.HomeDirectory + "/.cremona") {
			result = append(result, user)
		}
	}
	return result
}

func isExist(filename string) bool {
	_, err := os.Stat(filename)
	return err == nil
}
