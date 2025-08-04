package race

import (
	"brick_game/internal/fsm"
	"brick_game/internal/gametimer"
	"brick_game/internal/spec"
	"bufio"
	"math"
	"math/rand"
	"os"
	"strconv"
)

const (
	fieldHeight   = 20
	fieldWidth    = 10
	nextHeight    = 4
	nextWidth     = 4
	maxSpeed      = 10
	maxLevel      = 10
	levelUpScore  = 5
	startSpeed    = 500
	borderPattern = 4
)

type raceGame struct {
	state          spec.State
	action         spec.Action
	fsm            *fsm.FSM
	playerPos      car
	enemies        []car
	gameTimer      *gametimer.GameTimer
	currentSpeedMs int
	pendingSpawn   *bool
}

func (g *raceGame) UserInput(action spec.Action, hold bool) {
	g.action = action
}

func (g *raceGame) UpdateCurrentState() spec.State {
	g.fsm.Update()
	return g.state
}

func New() spec.Game {
	rg := &raceGame{
		state: spec.State{
			Field:     matrixInit(fieldHeight, fieldWidth),
			Next:      matrixInit(nextHeight, nextWidth),
			Pause:     true,
			Score:     0,
			HighScore: 0,
			Level:     1,
			Speed:     1,
		},
		action:         spec.Start,
		fsm:            fsm.New(GameStart),
		playerPos:      playerCarPositions.left,
		enemies:        []car{},
		gameTimer:      gametimer.New(startSpeed),
		currentSpeedMs: startSpeed,
		pendingSpawn:   nil,
	}

	// Регистрируем обработчики состояний
	rg.fsm.RegisterHandler(GameStart, &gameStartHandler{game: rg})
	rg.fsm.RegisterHandler(Spawn, &spawnHandler{game: rg})
	rg.fsm.RegisterHandler(Moving, &movingHandler{game: rg})
	rg.fsm.RegisterHandler(Shifting, &shiftingHandler{game: rg})
	rg.fsm.RegisterHandler(Attaching, &attachingHandler{game: rg})
	rg.fsm.RegisterHandler(GameOver, &gameOverHandler{game: rg})
	rg.fsm.RegisterHandler(Pause, &pauseHandler{game: rg})

	return rg
}

func (g *raceGame) startTimer() {
	g.gameTimer.Start()
}

func (g *raceGame) printBorders() {
	for i := range g.state.Field {
		if i%borderPattern != 0 {
			g.state.Field[i][0] = true
			g.state.Field[i][fieldWidth-1] = true
		}
	}
}

func (g *raceGame) printPlayer() {
	for _, v := range g.playerPos.coords {
		if v.y >= 0 && v.y < fieldHeight {
			g.state.Field[v.y][v.x] = true
		}
	}
}

/*
spawn methods
*/
func (g *raceGame) removeCarsOutOfField() bool {
	res := false
	if len(g.enemies) > 0 {
		filtered := g.enemies[:0]
		for _, enemy := range g.enemies {
			if !enemy.isOutOfField(fieldHeight) {
				filtered = append(filtered, enemy)
			} else {
				res = true
			}
		}
		g.enemies = filtered
	}
	return res
}

func (g *raceGame) addCarToEnemies() {
	if g.pendingSpawn != nil {
		if g.canSpawnCar(*g.pendingSpawn) {
			g.spawnCar(*g.pendingSpawn)
			g.pendingSpawn = nil
		}
	} else {
		spawnSide := randomSide()
		if g.canSpawnCar(spawnSide) {
			g.spawnCar(spawnSide)
		} else {
			g.pendingSpawn = &spawnSide
		}
	}
}

/*
moving methods
*/
func (g *raceGame) resetAction() {
	g.action = spec.Start
}

func (g *raceGame) togglePause() {
	g.state.Pause = !g.state.Pause
}

func (g *raceGame) isPaused() bool {
	return g.state.Pause
}

func (g *raceGame) isPauseAction() bool {
	return g.action == spec.Pause
}

func (g *raceGame) isGameoverAction() bool {
	return g.action == spec.Terminate
}

func (g *raceGame) handleUserMovement() {
	switch g.action {
	case spec.Left:
		g.playerPos.isRight = false
	case spec.Right:
		g.playerPos.isRight = true
	case spec.Up:
		g.gameTimer.FastReset()
	}
}

/*
shifting methods
*/
func (g *raceGame) updatePlayerCar() {
	var newCoords [carSize]coord
	if g.playerPos.isRight {
		newCoords = rightPlayerCoords
	} else {
		newCoords = leftPlayerCoords
	}
	g.swapCarPositions(g.playerPos.coords[:], newCoords[:])
	g.playerPos.coords = newCoords
}

func (g *raceGame) updateBorders() {
	j := 1
	for i := 0; i < fieldHeight-1; i += j {
		if !g.state.Field[i][0] {
			if i == borderPattern-1 {
				g.state.Field[0][0] = false
				g.state.Field[0][fieldWidth-1] = false
			}
			g.state.Field[i][0] = true
			g.state.Field[i][fieldWidth-1] = true
			g.state.Field[i+1][0] = false
			g.state.Field[i+1][fieldWidth-1] = false
			j = borderPattern
		}
	}
}

func (g *raceGame) renderEnemiesOnField(show bool) {
	for _, e := range g.enemies {
		for _, c := range e.coords {
			if c.y >= 0 && c.y < fieldHeight {
				g.state.Field[c.y][c.x] = show
			}
		}
	}
}

func (g *raceGame) shiftEnemies() {
	select {
	case <-g.gameTimer.TickChan():
		for i := range g.enemies {
			g.enemies[i].moveDown()
		}
		g.gameTimer.Reset(g.currentSpeedMs)
	default:
	}
}

func (g *raceGame) checkCollisions() bool {
	for _, e := range g.enemies {
		if g.playerPos.isCollidingWith(&e) {
			return true
		}
	}
	return false
}

/*
gameOver methods
*/
func (g *raceGame) stopTimer() {
	if g.gameTimer != nil {
		g.gameTimer.Stop()
	}
}

func (g *raceGame) levelToOver() {
	g.state.Level = -1
}

/*
utils
*/

func matrixInit(height int, length int) [][]bool {
	m := make([][]bool, height)
	for i := range m {
		m[i] = make([]bool, length)
	}
	return m
}

func (g *raceGame) swapCarPositions(from, to []coord) {
	for i := range from {
		g.state.Field[from[i].y][from[i].x] = false
		g.state.Field[to[i].y][to[i].x] = true
	}
}

func randomSide() bool {
	return rand.Intn(2) != 0
}

func (g *raceGame) spawnCar(isRight bool) {
	if isRight {
		g.enemies = append(g.enemies, enemyCarPositions.right)
	} else {
		g.enemies = append(g.enemies, enemyCarPositions.left)
	}
}

func (g *raceGame) canSpawnCar(spawnSide bool) bool {
	lenEnemies := len(g.enemies)
	if lenEnemies == 0 {
		return true
	}

	lastCar := g.enemies[lenEnemies-1]
	if spawnSide == lastCar.isRight {
		for _, e := range lastCar.coords {
			if e.y < 2 {
				return false
			}
		}
	} else {
		for _, e := range lastCar.coords {
			if e.y < 5 {
				return false
			}
		}
	}
	return true
}

func (g *raceGame) scoreIncrement() {
	g.state.Score++
	if g.levelUp() {
		g.speedUp()
	}
	if g.state.Score > g.state.HighScore {
		g.state.HighScore = g.state.Score
	}
}

func (g *raceGame) loadHighScore() {
	if file, err := os.Open("high_score.txt"); err == nil {
		defer file.Close()
		scanner := bufio.NewScanner(file)
		if scanner.Scan() {
			g.state.HighScore, _ = strconv.Atoi(scanner.Text())
		}
	}
}

func (g *raceGame) saveHighScore() {
	file, err := os.Create("high_score.txt")
	if err != nil {
		return
	}
	defer file.Close()
	file.WriteString(strconv.Itoa(g.state.HighScore))
}

func (g *raceGame) levelUp() bool {
	if g.state.Score%levelUpScore == 0 && g.state.Level < maxLevel {
		g.state.Level++
		return true
	}
	return false
}

func (g *raceGame) speedUp() bool {
	if g.state.Speed < maxSpeed {
		g.state.Speed++
		g.currentSpeedMs = int(float64(startSpeed) * math.Pow(0.8, float64(g.state.Speed)))
		g.gameTimer.Reset(g.currentSpeedMs)
		return true
	}
	return false
}

// func (g *raceGame) printCars() {
// 	for i, v := range g.enemies {
// 		println("car #", i, v.isRight)
// 		for _, c := range v.coords {
// 			println(c.y, " ", c.x)
// 		}
// 	}
// }
