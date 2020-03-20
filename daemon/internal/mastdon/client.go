package mastdon

import (
	"context"

	"github.com/ikihiki/cremona/daemon/internal/toot"
	"github.com/mattn/go-mastodon"
)

type Configrator interface {
	GetServer() string
	GetClientId() string
	GetClientSecret() string
	GetUsername() string
	GetPassword() string
}

type Client struct {
	config Configrator
	client *mastodon.Client
}

func NewMastdonCilent(config Configrator) (*Client, error) {
	client := mastodon.NewClient(&mastodon.Config{
		Server:       config.GetServer(),
		ClientID:     config.GetClientId(),
		ClientSecret: config.GetClientSecret(),
	})

	err := client.Authenticate(context.Background(), config.GetUsername(), config.GetPassword())
	if err != nil {
		return nil, err
	}
	return &Client{config: config, client: client}, nil
}

func (this *Client) SendToot(toot *toot.Toot) error {
	_, err := this.client.PostStatus(context.Background(), &mastodon.Toot{
		Status:     toot.GetText(),
		Visibility: "public",
	})
	return err
}

func getTimeline(client *mastodon.Client) ([]string, error) {
	timeline, err := client.GetTimelineHome(context.Background(), nil)
	if err != nil {
		return nil, err
	}
	var result []string
	for i := len(timeline) - 1; i >= 0; i-- {
		result = append(result, timeline[i].Content)
	}
	return result, nil
}
