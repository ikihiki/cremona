package agent

import (
	"io/ioutil"
	"path"

	"gopkg.in/yaml.v2"
)

type Config struct {
	Server       string `yaml:"server"`
	ClientId     string `yaml:"client_id"`
	ClientSecret string `yaml:"client_secret"`
	Username     string `yaml:"username"`
	Password     string `yaml:"password"`
	DriverName string `yaml:"driver_name"`
}

type ConfigLoad struct {
	baseDir string
	config *Config
}

func NewConfigLoad(baseDir string) (*ConfigLoad, error) {
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
	return &ConfigLoad{baseDir: baseDir, config: &config}, nil
}

func (this *ConfigLoad)GetConfig() *Config{
	return this.config
}