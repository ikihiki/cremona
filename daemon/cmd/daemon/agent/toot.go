package agent

import "strings"

type Toot struct {
	configLoader ConfigLoader
	builder strings.Builder
}

func (toot *Toot)AddStatusText(text string){
	toot.builder.WriteString(text)	
}