package master

import (
	"bufio"
	"os"
	"strconv"
	"strings"
)

type UserId int

type User struct {
	HomeDirectory string
	Username      string
	UserId        UserId
}

func searchHomeDir() []User {
	fp, err := os.Open("/etc/passwd")
	if err != nil {
		panic(err)
	}

	var result []User
	scanner := bufio.NewScanner(fp)
	for scanner.Scan() {
		fields := strings.Split(scanner.Text(), ":")
		userId, err := strconv.Atoi(fields[2])
		if err != nil {
			continue
		}
		user := User{
			HomeDirectory: fields[5],
			UserId:        UserId(userId),
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
