package master

import (
	"bufio"
	"os"
	"strconv"
	"strings"
)

type user struct {
	HomeDirectory string
	Username      string
	UserID        uint32
	GroupID       uint32
}

func searchHomeDir() []user {
	fp, err := os.Open("/etc/passwd")
	if err != nil {
		panic(err)
	}

	var result []user
	scanner := bufio.NewScanner(fp)
	for scanner.Scan() {
		fields := strings.Split(scanner.Text(), ":")
		userID, err := strconv.ParseUint(fields[2], 10, 32)
		if err != nil {
			continue
		}
		groupID, err := strconv.ParseUint(fields[3], 10, 32)
		if err != nil {
			continue
		}
		user := user{
			HomeDirectory: fields[5],
			UserID:        uint32(userID),
			GroupID:       uint32(groupID),
			Username:      fields[0],
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
