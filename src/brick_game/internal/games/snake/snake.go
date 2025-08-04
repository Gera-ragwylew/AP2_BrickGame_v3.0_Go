package snake

import (
	"brick_game/internal/games/snake/cpp_wrapper"
	"brick_game/internal/spec"
)

type Snake struct {
	state  spec.State
	action spec.Action
}

func New() spec.Game {
	s := &Snake{
		state:  spec.State{},
		action: spec.Start,
	}

	/*
		spec.Terminate is a 2nd action in UserInput
		and the Snake is 2nd game in game_fabric.h
		first call need to initialize the game Snake
	*/
	cpp_wrapper.UserInput(spec.Terminate, false)
	return s
}

func (s *Snake) UserInput(action spec.Action, hold bool) {
	cpp_wrapper.UserInput(action, hold)
}

func (s *Snake) UpdateCurrentState() spec.State {
	return cpp_wrapper.UpdateCurrentState()
}
