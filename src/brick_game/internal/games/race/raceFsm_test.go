package race

import (
	"brick_game/internal/fsm"
	gamespec "brick_game/internal/game_spec"
	"testing"
)

func TestRaceFSMTransitions(t *testing.T) {
	tests := []struct {
		name           string
		setupGame      func(*raceGame)
		expectedState  fsm.State
		checkGameState func(*raceGame, *testing.T)
	}{
		{
			name:      "GameStart to Spawn transition",
			setupGame: func(mg *raceGame) {},

			expectedState: Spawn,
			checkGameState: func(mg *raceGame, t *testing.T) {
				if mg.fsm.GetState() != Spawn {
					t.Errorf("Expected state Spawn, got %v", mg.fsm.GetState())
				}
			},
		},
		{
			name: "Spawn to Moving transition with cars removed",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Spawn)
			},
			expectedState: Moving,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Moving to GameOver on game over action",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Moving)
				mg.action = gamespec.Terminate
			},
			expectedState: GameOver,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Moving to Pause on pause action",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Moving)
				mg.state.Pause = false
				mg.action = gamespec.Pause
			},
			expectedState: Pause,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Moving to Shifting normally",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Moving)
				mg.state.Pause = false
			},
			expectedState: Shifting,
			checkGameState: func(mg *raceGame, t *testing.T) {

			},
		},
		{
			name: "Pause to GameOver on game over action",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Moving)
				mg.state.Pause = true
				mg.action = gamespec.Terminate
			},
			expectedState: GameOver,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Pause to Moving on pause action",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Pause)
				mg.state.Pause = true
				mg.action = gamespec.Pause
			},
			expectedState: Moving,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Pause remains in Pause without actions",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Pause)
				mg.state.Pause = true
				mg.action = gamespec.Right // or any other except gamespec.Pause
			},
			expectedState: Pause,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Shifting to Attaching transition",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Shifting)
			},
			expectedState: Attaching,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Attaching to GameOver on collision",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Attaching)
				mg.playerPos = newCar(false, [7]coord{
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}}) // for example to collide
				mg.enemies = append(mg.enemies, newCar(false, [7]coord{
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}, {y: 13, x: 13},
					{y: 13, x: 13}}))
			},
			expectedState: GameOver,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "Attaching to Spawn without collision",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(Attaching)
			},
			expectedState: Spawn,
			checkGameState: func(mg *raceGame, t *testing.T) {
			},
		},
		{
			name: "GameOver remains in GameOver",
			setupGame: func(mg *raceGame) {
				mg.fsm.SetState(GameOver)
			},
			expectedState: GameOver,
			checkGameState: func(mg *raceGame, t *testing.T) {
				if mg.state.Level != -1 {
					t.Errorf("Expected level %v, got %v", -1, mg.state.Level)
				}
			},
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			game := createTestRaceGame()
			tt.setupGame(game)

			game.fsm.Update()

			if game.fsm.GetState() != tt.expectedState {
				t.Errorf("Expected state %v, got %v", tt.expectedState, game.fsm.GetState())
			}

			if tt.checkGameState != nil {
				tt.checkGameState(game, t)
			}
		})
	}
}
