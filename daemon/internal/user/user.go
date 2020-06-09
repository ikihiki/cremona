package user

import (
	"bufio"
	"os"
	"os/user"
	"path"
	"strings"
)

var passwdFilePath = "/etc/passwd"
func GetUsers() ([]user.User, error) {
	passwdFile, err := os.Open(passwdFilePath)
	if err != nil {
		return nil, err
	}
	defer passwdFile.Close()

	result := []user.User{}
	scanner := bufio.NewScanner(passwdFile)
	for scanner.Scan() {
	if err := scanner.Err(); err != nil {
			return nil, err
		}

		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		usr := parseLine(line)
		result = append(result, usr)
	}
	return result, nil
}

func parseLine(line string ) user.User {
	user := user.User{}

	// see: man 5 passwd
	//  name:password:UID:GID:GECOS:directory:shell
	parts := strings.Split(line, ":")
	if len(parts) >= 1 {
		user.Username = parts[0]
		user.Name = parts[0]
	}
	if len(parts) >= 3 {
		user.Uid = parts[2]
	}
	if len(parts) >= 4 {
		user.Gid = parts[3]
	}
	if len(parts) >= 6 {
		user.HomeDir = parts[5]
	}

	return user
}

func exists(filename string) bool {
	_, err := os.Stat(filename)
	return err == nil
}

func GetUidsHaveConfig(fileName string ) ([]user.User, error){
	users, err := GetUsers()
	if err != nil {
		return nil, err
	}

	results := []user.User{}
	for _, usr := range users {
		cfg := path.Join(usr.HomeDir, fileName)
		if exists(cfg) {
			results = append(results, usr)
		}
	}
	return results, nil
}