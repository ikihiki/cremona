package config

import (
	"io/ioutil"
	"os"
	"path"

	"gopkg.in/yaml.v2"
)

type Config struct {
	Server       string `yaml:"server"`
	ClientId     string `yaml:"client_id"`
	ClientSecret string `yaml:"client_secret"`
	Username     string `yaml:"username"`
	Password     string `yaml:"password"`
	DeviceName   string `yaml:"device_name"`
}

func LoadConfig(baseDir string) (*Config, error) {
	configPath := path.Join(baseDir, ".cremona")
	buf, err := ioutil.ReadFile(configPath)
	if err != nil {
		return nil, err
	}
	config := Config{}
	err = yaml.Unmarshal(buf, &config)
	if err != nil {
		return nil, err
	}
	return &config, nil
}

func (this *Config) GetServer() string {
	return this.Server
}
func (this *Config) GetClientId() string {
	return this.ClientId
}
func (this *Config) GetClientSecret() string {
	return this.ClientSecret
}
func (this *Config) GetUsername() string {
	return this.Username
}
func (this *Config) GetPassword() string {
	return this.Password
}
func (this *Config) GetDeviceName() string {
	return this.DeviceName
}
func (this *Config) GetUserId() uint32 {
	return uint32(os.Getuid())
}