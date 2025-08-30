package race

import (
	"brick_game/internal/fsm"
)

const (
	GameStart fsm.State = "GameStart"
	Spawn     fsm.State = "Spawn"
	Moving    fsm.State = "Moving"
	Pause     fsm.State = "Pause"
	Shifting  fsm.State = "Shifting"
	Attaching fsm.State = "Attaching"
	GameOver  fsm.State = "GameOver"
)

type gameStartHandler struct {
	game *raceGame
}

func (h *gameStartHandler) Handle() {
	h.game.startTimer()
	h.game.loadHighScore()
	h.game.printBorders()
	h.game.printPlayer()
	h.game.fsm.SetState(Spawn)
}

type spawnHandler struct {
	game *raceGame
}

func (h *spawnHandler) Handle() {
	if h.game.removeCarsOutOfField() {
		h.game.scoreIncrement()
	}
	h.game.addCarToEnemies()
	h.game.fsm.SetState(Moving)
}

type movingHandler struct {
	game *raceGame
}

func (h *movingHandler) Handle() {
	defer h.game.resetAction()

	if h.game.isGameoverAction() {
		h.game.fsm.SetState(GameOver)
		return
	}
	if h.game.isPauseAction() {
		h.game.togglePause()
	}
	if h.game.isPaused() {
		h.game.fsm.SetState(Pause)
		return
	}
	h.game.handleUserMovement()

	h.game.fsm.SetState(Shifting)
}

type pauseHandler struct {
	game *raceGame
}

func (h *pauseHandler) Handle() {
	defer h.game.resetAction()
	if h.game.isGameoverAction() {
		h.game.fsm.SetState(GameOver)
		return
	}
	if h.game.isPauseAction() {
		h.game.togglePause()
		h.game.fsm.SetState(Moving)
		return
	}
	h.game.fsm.SetState(Pause)
}

type shiftingHandler struct {
	game *raceGame
}

func (h *shiftingHandler) Handle() {
	h.game.updatePlayerCar()
	h.game.updateBorders()
	h.game.renderEnemiesOnField(false)
	h.game.shiftEnemies()
	h.game.renderEnemiesOnField(true)
	h.game.fsm.SetState(Attaching)
}

type attachingHandler struct {
	game *raceGame
}

func (h *attachingHandler) Handle() {
	if h.game.checkCollisions() {
		h.game.fsm.SetState(GameOver)
	} else {
		h.game.fsm.SetState(Spawn)
	}
}

type gameOverHandler struct {
	game *raceGame
}

func (h *gameOverHandler) Handle() {
	h.game.saveHighScore()
	h.game.stopTimer()
	h.game.levelToOver()
}
