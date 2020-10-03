package toot

import (
	"strings"
)

type Toot struct {
	Id   uint32
	Text *strings.Builder
}

func (this *Toot) GetText() string {
	return this.Text.String()
}
