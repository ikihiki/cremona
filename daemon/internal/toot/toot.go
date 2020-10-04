package toot

import (
	"sort"
	"strings"
)

type element struct {
	index uint32
	text string
}


type Toot struct {
	id    uint32
	elements []element
}

func (toot *Toot) addElement(index uint32, text string){
	toot.elements = append(toot.elements, element{index: index, text: text})
}

func (toot *Toot) GetText() string {
	sort.Slice(toot.elements, func(i, j int) bool {return toot.elements[i].index < toot.elements[j].index})
	builder := strings.Builder{}
	for _, element := range toot.elements {
		builder.WriteString(element.text)
	}

	return builder.String()
}
