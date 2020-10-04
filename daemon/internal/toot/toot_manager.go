package toot

import (
	"errors"
)

type TootManager interface {
	NewToot(id uint32) error
	AddTootElement(id uint32, index uint32, text string) (int, error)
	SendToot(id uint32) error
}

type Configrator interface {
}

type TootSender interface {
	SendToot(toot *Toot) error
}

type TootManage struct {
	toots  map[uint32]*Toot
	config Configrator
	sender TootSender
}

func NewTootManage(config Configrator, sender TootSender) (*TootManage, error) {
	tootManage := &TootManage{toots: make(map[uint32]*Toot), config: config, sender: sender}
	return tootManage, nil
}

func (this *TootManage) NewToot(id uint32) error {
	if _, ok := this.toots[id]; !ok {
		this.toots[id] = &Toot{id: id, elements: make([]element, 0)}
		return nil
	} else {
		return errors.New("Already Have id")
	}
}

func (this *TootManage) AddTootElement(id uint32, index uint32, text string) (int, error) {
	if val, ok := this.toots[id]; ok {
		 val.addElement(index, text)
		 return len(text), nil
	} else {
		return 0, errors.New("No toot")
	}
}

func (this *TootManage) SendToot(id uint32) error {
	if val, ok := this.toots[id]; ok {
		return this.sender.SendToot(val)
	} else {
		return errors.New("No toot")
	}
}
