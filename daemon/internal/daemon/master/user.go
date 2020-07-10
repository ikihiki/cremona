package master

import (
	"bufio"
	"os"
	"strconv"
	"strings"
)



func searchHomeDir() []user {
	fp, err := os.Open("/etc/passwd")
	if err != nil {
		panic(err)
	}

	var result []user
	scanner := bufio.NewScanner(fp)
	for scanner.Scan() {
		fields := strings.Split(scanner.Text(), ":")
		id, err := strconv.Atoi(fields[2])
		if err != nil {
			continue
		}
		user := user{
			HomeDirectory: fields[5],
			UserID:        userID(id),
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
