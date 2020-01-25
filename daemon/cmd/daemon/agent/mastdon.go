package agent

import (
	"context"
	"log"

	"github.com/mattn/go-mastodon"
)

type Mastdon struct {
	configLoader ConfigLoader
	client *mastodon.Client
}

func NewMastdonCilent(configLoader ConfigLoader) (*Mastdon, error){
	config := configLoader.GetConfig()
	client := mastodon.NewClient(&mastodon.Config{
		Server:       config.Server,
		ClientID:     config.ClientId,
		ClientSecret: config.ClientSecret,
	})

	err := client.Authenticate(context.Background(), config.Username, config.Password)
	if err != nil {
		return nil, err
	}
	return &Mastdon{configLoader: configLoader, client: client},nil
}

func (mastdon *Mastdon)Toot(toot *Toot) {
	mastdon.client.PostStatus(context.Background(), &mastodon.Toot{
		Status:     toot.builder.String(),
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
