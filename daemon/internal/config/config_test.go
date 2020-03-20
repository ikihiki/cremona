package config_test

import (
	"testing"

	"github.com/ikihiki/cremona/daemon/internal/config"
	"github.com/stretchr/testify/assert"
)


func TestConfig_LoadConfig(t *testing.T) {
	config, err := config.LoadConfig("./testdata")

	assert.Nil(t, err)
	assert.Equal(t, "https://server.com", config.Server)
	assert.Equal(t, "client", config.ClientId)
	assert.Equal(t, "secret", config.ClientSecret)
	assert.Equal(t, "name", config.Username)
	assert.Equal(t, "pass", config.Password)
	assert.Equal(t, "aa", config.DeviceName)
}
