package race

import (
	"brick_game/internal/fsm"
	gamespec "brick_game/internal/game_spec"
	gametimer "brick_game/internal/game_timer"
)

func createTestRaceGame() *raceGame {
	rg := &raceGame{
		state: gamespec.State{
			Field:     matrixInit(fieldHeight, fieldWidth),
			Next:      matrixInit(nextHeight, nextWidth),
			Pause:     true,
			Score:     0,
			HighScore: 0,
			Level:     1,
			Speed:     1,
		},
		playerPos:      playerCarPositions.left,
		fsm:            fsm.New(GameStart),
		enemies:        []car{},
		currentSpeedMs: startSpeed,
		gameTimer:      gametimer.New(startSpeed),
	}

	rg.fsm.RegisterHandler(GameStart, &gameStartHandler{game: rg})
	rg.fsm.RegisterHandler(Spawn, &spawnHandler{game: rg})
	rg.fsm.RegisterHandler(Moving, &movingHandler{game: rg})
	rg.fsm.RegisterHandler(Shifting, &shiftingHandler{game: rg})
	rg.fsm.RegisterHandler(Attaching, &attachingHandler{game: rg})
	rg.fsm.RegisterHandler(GameOver, &gameOverHandler{game: rg})
	rg.fsm.RegisterHandler(Pause, &pauseHandler{game: rg})
	return rg
}
