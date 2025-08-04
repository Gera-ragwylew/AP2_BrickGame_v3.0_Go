package games

import (
	"brick_game/internal/games/race"
	snake "brick_game/internal/games/snake"
	tetris "brick_game/internal/games/tetris"
	"brick_game/internal/spec"
	"fmt"
)

var gamesList = []struct {
	ID   string
	Name string
}{
	{"1", "race"},
	{"2", "tetris"},
	{"3", "snake"},
}

func GetGamesList() []struct{ ID, Name string } {
	return gamesList
}

func CreateGame(gameId string) (spec.Game, error) {
	switch gameId {
	case "1":
		return race.New(), nil
	case "2":
		return tetris.New(), nil
	case "3":
		return snake.New(), nil
	default:
		return nil, fmt.Errorf("unknown game type")
	}
}
