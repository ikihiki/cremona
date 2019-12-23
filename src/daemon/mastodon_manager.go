package main

import (
	"context"
	"io/ioutil"
	"log"

	"github.com/go-yaml/yaml"
	"github.com/mattn/go-mastodon"
)

func RunServier(userInfo UserInfo) {
	//config := loadUserConfig(userInfo)
	//client := createCilent(config)
	//client.GetAccountCurrentUser(context)
}

func loadUserConfig(userInfo UserInfo) Config {
	config := Config{}

	buf, err := ioutil.ReadFile("./config.yaml")
	if err != nil {
		panic(err)
	}

	err = yaml.Unmarshal(buf, &config)
	if err != nil {
		panic(err)
	}

	return config
}

func createCilent(config Config) *mastodon.Client {
	c := mastodon.NewClient(&mastodon.Config{
		Server:       config.Server,
		ClientID:     config.ClientId,
		ClientSecret: config.ClientSecret,
	})

	err := c.Authenticate(context.Background(), config.Username, config.Password)
	if err != nil {
		log.Fatal(err)
	}
	return c
}

func toot(client *mastodon.Client, status string) {
	client.PostStatus(context.Background(), &mastodon.Toot{
		Status:     status,
		Visibility: "public",
	})
}

func getTimeline(client *mastodon.Client) []string {
	timeline, err := client.GetTimelineHome(context.Background(), nil)
	if err != nil {
		log.Fatal(err)
	}
	var result []string
	for i := len(timeline) - 1; i >= 0; i-- {
		result = append(result, timeline[i].Content)
	}
	return result
}


