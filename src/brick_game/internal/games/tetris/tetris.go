package tetris

import (
	"brick_game/internal/games/tetris/wrapper"
	"brick_game/internal/spec"
)

type Tetris struct {
	state  spec.State
	action spec.Action
}

func New() spec.Game {
	return &Tetris{
		state:  spec.State{},
		action: spec.Start,
	}
}

func (t *Tetris) UserInput(action spec.Action, hold bool) {
	wrapper.UserInput(action, hold)
}

func (t *Tetris) UpdateCurrentState() spec.State {
	return wrapper.UpdateCurrentState()
}
