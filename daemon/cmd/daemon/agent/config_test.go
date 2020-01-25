package agent_test

import "testing"

import "github.com/ikihiki/cremona/daemon/agent"

import "github.com/stretchr/testify/assert"


func TestConfig_LoadConfig(t *testing.T) {
	configLoad, err := agent.NewConfigLoad("./testdata")
	config := configLoad.GetConfig()

	assert.Nil(t, err)
	assert.Equal(t, "https://server.com", config.Server)
	assert.Equal(t, "client", config.ClientId)
	assert.Equal(t, "secret", config.ClientSecret)
	assert.Equal(t, "name", config.Username)
	assert.Equal(t, "pass", config.Password)
}
