package gamefabric

import (
	"brick_game/internal/adapter"
	gamespec "brick_game/internal/game_spec"
	"brick_game/internal/games/race"
	"fmt"
)

var gamesList = []struct {
	ID   int
	Name string
}{
	{1, "race"},
	{2, "tetris"},
	{3, "snake"},
}

func GetGamesList() []struct {
	ID   int
	Name string
} {
	return gamesList
}

func CreateGame(gameId int) (gamespec.Game, error) {
	switch gameId {
	case 1:
		return race.New(), nil
	case 2:
		return adapter.New("tetris"), nil
	case 3:
		return adapter.New("snake"), nil
	default:
		return nil, fmt.Errorf("game not found")
	}
}
