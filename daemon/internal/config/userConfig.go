package config

import (
	"io/ioutil"
	"os"
	"path"

	"github.com/ikihiki/cremona/daemon/internal/daemon"
	"gopkg.in/yaml.v2"
)

type UserConfig struct {
	Server       string `yaml:"server"`
	ClientId     string `yaml:"client_id"`
	ClientSecret string `yaml:"client_secret"`
	Username     string `yaml:"username"`
	Password     string `yaml:"password"`
	DeviceName   string `yaml:"device_name"`
}

type UserConfigLoad struct{}

func (UserConfigLoad) LoadConfig(baseDir string) (daemon.UserConfigrator, error) {
	configPath := path.Join(baseDir, ".cremona")
	buf, err := ioutil.ReadFile(configPath)
	if err != nil {
		return nil, err
	}
	config := UserConfig{}
	err = yaml.Unmarshal(buf, &config)
	if err != nil {
		return nil, err
	}
	return &config, nil
}

func (this *UserConfig) GetServer() string {
	return this.Server
}
func (this *UserConfig) GetClientId() string {
	return this.ClientId
}
func (this *UserConfig) GetClientSecret() string {
	return this.ClientSecret
}
func (this *UserConfig) GetUsername() string {
	return this.Username
}
func (this *UserConfig) GetPassword() string {
	return this.Password
}
func (this *UserConfig) GetDeviceName() string {
	return this.DeviceName
}
func (this *UserConfig) GetUserId() uint32 {
	return uint32(os.Getuid())
}