package race

import (
	"encoding/binary"
	"os"
	"testing"

	gamespec "brick_game/internal/game_spec"
	gametimer "brick_game/internal/game_timer"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
	"github.com/stretchr/testify/suite"
)

type RaceGameTestSuite struct {
	suite.Suite
	game *raceGame
}

func (s *RaceGameTestSuite) SetupTest() {
	s.game = &raceGame{
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
		enemies:        []car{},
		gameTimer:      gametimer.New(startSpeed),
		currentSpeedMs: startSpeed,
		pendingSpawn:   nil,
	}
}

func TestRaceGameTestSuite(t *testing.T) {
	suite.Run(t, new(RaceGameTestSuite))
}

func (s *RaceGameTestSuite) TestMatrixInit() {
	t := s.T()

	matrix := matrixInit(3, 4)
	assert.Len(t, matrix, 3)
	assert.Len(t, matrix[0], 4)
	assert.Len(t, matrix[1], 4)
	assert.Len(t, matrix[2], 4)

	for i := 0; i < 3; i++ {
		for j := 0; j < 4; j++ {
			assert.False(t, matrix[i][j])
		}
	}

	emptyMatrix := matrixInit(0, 0)
	assert.Empty(t, emptyMatrix)
}

func (s *RaceGameTestSuite) TestLoadHighScore() {
	t := s.T()
	testFile := "race_data.bin"
	defer os.Remove(testFile)

	tests := []struct {
		name         string
		setupValue   int32
		initialScore int
		expected     int
		shouldCreate bool
	}{
		{
			name:         "File exists with positive score",
			setupValue:   1500,
			initialScore: 100,
			expected:     1500,
			shouldCreate: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Setup
			if tt.shouldCreate {
				file, err := os.Create(testFile)
				require.NoError(t, err)
				err = binary.Write(file, binary.LittleEndian, tt.setupValue)
				require.NoError(t, err)
				file.Close()
			}

			s.game.loadHighScore()
			assert.Equal(t, tt.expected, s.game.state.HighScore)
		})
	}
}

func (s *RaceGameTestSuite) TestSaveHighScore() {
	t := s.T()
	testFile := "race_data.bin"

	defer os.Remove(testFile)

	tests := []struct {
		name     string
		score    int
		expected int32
	}{
		{"Positive score", 2500, 2500},
		{"Zero score", 0, 0},
		{"Negative score", -500, -500},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.state.HighScore = tt.score
			s.game.saveHighScore()
			file, err := os.Open(testFile)
			require.NoError(t, err)

			var savedValue int32
			err = binary.Read(file, binary.LittleEndian, &savedValue)
			require.NoError(t, err)
			assert.Equal(t, tt.expected, savedValue)
		})
	}
}

func (s *RaceGameTestSuite) TestLevelUp() {
	t := s.T()

	tests := []struct {
		name           string
		initialScore   int
		initialLevel   int
		expectedLevel  int
		expectedReturn bool
	}{
		{"Level up at 5 points", 5, 1, 2, true},
		{"Level up at 10 points", 10, 2, 3, true},
		{"No level up at 4 points", 4, 1, 1, false},
		{"No level up at max level", 50, 10, 10, false},
		{"Level up from level 5", 25, 5, 6, true},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.state.Score = tt.initialScore
			s.game.state.Level = tt.initialLevel

			result := s.game.levelUp()

			assert.Equal(t, tt.expectedReturn, result)
			assert.Equal(t, tt.expectedLevel, s.game.state.Level)
		})
	}
}

func (s *RaceGameTestSuite) TestSpeedUp() {
	t := s.T()

	tests := []struct {
		name           string
		initialSpeed   int
		initialLevel   int
		expectedSpeed  int
		expectedReturn bool
	}{
		{"Speed up from level 1", 1, 1, 2, true},
		{"Speed up from level 5", 5, 5, 6, true},
		{"No speed up at max speed", 10, 10, 10, false},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.state.Speed = tt.initialSpeed
			s.game.state.Level = tt.initialLevel
			s.game.currentSpeedMs = startSpeed

			result := s.game.speedUp()

			assert.Equal(t, tt.expectedReturn, result)
			assert.Equal(t, tt.expectedSpeed, s.game.state.Speed)
			assert.True(t, s.game.currentSpeedMs < startSpeed || tt.initialSpeed == maxSpeed)
		})
	}
}

func (s *RaceGameTestSuite) TestScoreIncrement() {
	t := s.T()

	tests := []struct {
		name              string
		initialScore      int
		initialHighScore  int
		initialLevel      int
		expectedScore     int
		expectedHighScore int
		expectedLevel     int
	}{
		{"Normal increment", 10, 20, 2, 11, 20, 2},
		{"Level up increment", 4, 5, 1, 5, 5, 2},
		{"New high score", 15, 10, 3, 16, 16, 3},
		{"Level up and new high score", 9, 8, 2, 10, 10, 3},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.state.Score = tt.initialScore
			s.game.state.HighScore = tt.initialHighScore
			s.game.state.Level = tt.initialLevel

			s.game.scoreIncrement()

			assert.Equal(t, tt.expectedScore, s.game.state.Score)
			assert.Equal(t, tt.expectedHighScore, s.game.state.HighScore)
			assert.Equal(t, tt.expectedLevel, s.game.state.Level)
		})
	}
}

func (s *RaceGameTestSuite) TestCanSpawnCar() {
	t := s.T()

	tests := []struct {
		name           string
		enemiesSetup   []car
		spawnSide      bool
		expectedResult bool
	}{
		{
			name:           "No enemies - can spawn",
			enemiesSetup:   []car{},
			spawnSide:      true,
			expectedResult: true,
		},
		{
			name: "Same side - last car too high",
			enemiesSetup: []car{
				{isRight: true, coords: [carSize]coord{{x: 1, y: 1}, {x: 1, y: 1}, {x: 1, y: 1}, {x: 1, y: 1}, {x: 1, y: 1}, {x: 1, y: 1}, {x: 1, y: 1}}},
			},
			spawnSide:      true,
			expectedResult: false,
		},
		{
			name: "Different side - last car too high",
			enemiesSetup: []car{
				{isRight: false, coords: [carSize]coord{{x: 1, y: 3}, {x: 1, y: 3}, {x: 1, y: 3}, {x: 1, y: 3}, {x: 1, y: 3}, {x: 1, y: 3}, {x: 1, y: 3}}},
			},
			spawnSide:      true,
			expectedResult: false,
		},
		{
			name: "Different side - last car low enough",
			enemiesSetup: []car{
				{isRight: false, coords: [carSize]coord{{y: 6}, {y: 6}, {y: 6}, {y: 6}, {y: 6}, {y: 6}, {y: 6}}},
			},
			spawnSide:      true,
			expectedResult: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.enemies = tt.enemiesSetup

			result := s.game.canSpawnCar(tt.spawnSide)

			assert.Equal(t, tt.expectedResult, result)
		})
	}
}

func (s *RaceGameTestSuite) TestRemoveCarsOutOfField() {
	t := s.T()

	tests := []struct {
		name            string
		enemiesSetup    []car
		expectedEnemies int
		expectedReturn  bool
	}{
		{
			name: "No cars out of field",
			enemiesSetup: []car{
				{coords: [carSize]coord{{y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}}},
				{coords: [carSize]coord{{y: 10}, {y: 10}, {y: 10}, {y: 10}, {y: 10}, {y: 10}, {y: 10}}},
			},
			expectedEnemies: 2,
			expectedReturn:  false,
		},
		{
			name: "Some cars out of field",
			enemiesSetup: []car{
				{coords: [carSize]coord{{y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}, {y: 5}}},
				{coords: [carSize]coord{{y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}}}, // Out of field
			},
			expectedEnemies: 1,
			expectedReturn:  true,
		},
		{
			name: "All cars out of field",
			enemiesSetup: []car{
				{coords: [carSize]coord{{y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}, {y: 25}}},
				{coords: [carSize]coord{{y: 30}, {y: 30}, {y: 30}, {y: 30}, {y: 30}, {y: 30}, {y: 30}}},
			},
			expectedEnemies: 0,
			expectedReturn:  true,
		},
		{
			name:            "No enemies",
			enemiesSetup:    []car{},
			expectedEnemies: 0,
			expectedReturn:  false,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.enemies = tt.enemiesSetup

			result := s.game.removeCarsOutOfField()

			assert.Equal(t, tt.expectedEnemies, len(s.game.enemies))
			assert.Equal(t, tt.expectedReturn, result)
		})
	}
}

func (s *RaceGameTestSuite) TestTogglePause() {
	t := s.T()

	s.game.state.Pause = true
	s.game.togglePause()
	assert.False(t, s.game.state.Pause)

	s.game.togglePause()
	assert.True(t, s.game.state.Pause)
}

func (s *RaceGameTestSuite) TestIsPaused() {
	t := s.T()

	s.game.state.Pause = true
	assert.True(t, s.game.isPaused())

	s.game.state.Pause = false
	assert.False(t, s.game.isPaused())
}

func (s *RaceGameTestSuite) TestResetAction() {
	t := s.T()

	s.game.action = gamespec.Left
	s.game.resetAction()
	assert.Equal(t, gamespec.Start, s.game.action)
}

func (s *RaceGameTestSuite) TestRandomSide() {
	t := s.T()

	trueCount := 0
	falseCount := 0

	for i := 0; i < 100; i++ {
		result := randomSide()
		if result {
			trueCount++
		} else {
			falseCount++
		}
	}

	assert.True(t, trueCount > 20)
	assert.True(t, falseCount > 20)
}

func (s *RaceGameTestSuite) TestSpawnCar() {
	t := s.T()

	initialCount := len(s.game.enemies)

	s.game.spawnCar(true)
	assert.Len(t, s.game.enemies, initialCount+1)
	assert.True(t, s.game.enemies[0].isRight)

	s.game.spawnCar(false)
	assert.Len(t, s.game.enemies, initialCount+2)
	assert.False(t, s.game.enemies[1].isRight)
}

func (s *RaceGameTestSuite) TestPrintBorders() {
	t := s.T()

	s.game.state.Field = matrixInit(fieldHeight, fieldWidth)

	s.game.printBorders()

	for i := range s.game.state.Field {
		if i%borderPattern != 0 {
			assert.True(t, s.game.state.Field[i][0])
			assert.True(t, s.game.state.Field[i][fieldWidth-1])
		} else {
			assert.False(t, s.game.state.Field[i][0])
			assert.False(t, s.game.state.Field[i][fieldWidth-1])
		}
	}
}

func (s *RaceGameTestSuite) TestUpdateBorders() {
	t := s.T()

	s.game.state.Field = matrixInit(fieldHeight, fieldWidth)

	s.game.updateBorders()

	for i := 0; i < fieldHeight-1; i += borderPattern {
		assert.True(t, s.game.state.Field[i][0])
		assert.True(t, s.game.state.Field[i][fieldWidth-1])

		if i+1 < fieldHeight {
			assert.False(t, s.game.state.Field[i+1][0])
			assert.False(t, s.game.state.Field[i+1][fieldWidth-1])
		}
	}

	assert.True(t, s.game.state.Field[0][0])
	assert.True(t, s.game.state.Field[0][fieldWidth-1])
	assert.False(t, s.game.state.Field[1][0])
	assert.False(t, s.game.state.Field[1][fieldWidth-1])

	assert.True(t, s.game.state.Field[4][0])
	assert.True(t, s.game.state.Field[4][fieldWidth-1])
	assert.False(t, s.game.state.Field[5][0])
	assert.False(t, s.game.state.Field[5][fieldWidth-1])

	assert.True(t, s.game.state.Field[8][0])
	assert.True(t, s.game.state.Field[8][fieldWidth-1])
	assert.False(t, s.game.state.Field[9][0])
	assert.False(t, s.game.state.Field[9][fieldWidth-1])

	for i := 0; i < fieldHeight; i++ {
		for j := 1; j < fieldWidth-1; j++ {
			assert.False(t, s.game.state.Field[i][j])
		}
	}
}

func (s *RaceGameTestSuite) TestPrintPlayer() {
	t := s.T()

	s.game.state.Field = matrixInit(fieldHeight, fieldWidth)

	s.game.printPlayer()

	for _, coord := range s.game.playerPos.coords {
		if coord.y >= 0 && coord.y < fieldHeight {
			assert.True(t, s.game.state.Field[coord.y][coord.x])
		}
	}
}

func (s *RaceGameTestSuite) TestUpdatePlayerCar() {
	t := s.T()

	initialCoords := s.game.playerPos.coords

	s.game.state.Field = matrixInit(fieldHeight, fieldWidth)
	for _, coord := range initialCoords {
		if coord.y >= 0 && coord.y < fieldHeight {
			s.game.state.Field[coord.y][coord.x] = true
		}
	}

	s.game.playerPos.isRight = true
	s.game.updatePlayerCar()

	assert.Equal(t, rightPlayerCoords, s.game.playerPos.coords)
	assert.True(t, s.game.playerPos.isRight)

	for _, coord := range initialCoords {
		if coord.y >= 0 && coord.y < fieldHeight {
			assert.False(t, s.game.state.Field[coord.y][coord.x])
		}
	}
	for _, coord := range rightPlayerCoords {
		if coord.y >= 0 && coord.y < fieldHeight {
			assert.True(t, s.game.state.Field[coord.y][coord.x])
		}
	}
}

func (s *RaceGameTestSuite) TestCheckCollisions() {
	t := s.T()

	tests := []struct {
		name           string
		enemiesSetup   []car
		playerPos      car
		expectedResult bool
	}{
		{
			name:           "No collisions",
			enemiesSetup:   []car{{coords: [carSize]coord{{x: 5, y: 5}, {x: 6, y: 5}, {x: 5, y: 6}, {x: 6, y: 6}, {x: 6, y: 6}, {x: 6, y: 6}, {x: 6, y: 6}}}},
			playerPos:      playerCarPositions.left,
			expectedResult: false,
		},
		{
			name:           "Collision detected",
			enemiesSetup:   []car{{isRight: false, coords: [carSize]coord{{x: 1, y: 18}, {x: 2, y: 18}, {x: 1, y: 19}, {x: 2, y: 19}, {x: 2, y: 19}, {x: 2, y: 19}, {x: 2, y: 19}}}},
			playerPos:      playerCarPositions.left,
			expectedResult: true,
		},
		{
			name:           "No enemies",
			enemiesSetup:   []car{},
			playerPos:      playerCarPositions.left,
			expectedResult: false,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.enemies = tt.enemiesSetup
			s.game.playerPos = tt.playerPos

			result := s.game.checkCollisions()

			assert.Equal(t, tt.expectedResult, result)
		})
	}
}

func (s *RaceGameTestSuite) TestAddCarToEnemies() {
	t := s.T()

	t.Run("Test addCarToEnemies scenarios", func(t *testing.T) {
		game := createTestRaceGame()

		game.enemies = []car{}
		game.pendingSpawn = nil

		initialCount := len(game.enemies)
		game.addCarToEnemies()

		assert.Equal(t, initialCount+1, len(game.enemies))
		assert.Nil(t, game.pendingSpawn)
	})

	t.Run("Test pending spawn logic", func(t *testing.T) {
		game := createTestRaceGame()

		pending := true
		game.pendingSpawn = &pending
		game.enemies = []car{}

		initialCount := len(game.enemies)
		game.addCarToEnemies()

		assert.Equal(t, initialCount+1, len(game.enemies))
		assert.Nil(t, game.pendingSpawn)
	})
}

func (s *RaceGameTestSuite) TestRenderEnemiesOnField() {
	t := s.T()

	tests := []struct {
		name        string
		enemies     []car
		show        bool
		expected    [][]bool
		description string
	}{
		{
			name: "Show enemies on empty field",
			enemies: []car{
				{
					coords: [carSize]coord{
						{x: 1, y: 1}, {x: 2, y: 1},
						{x: 1, y: 2}, {x: 2, y: 2},
						{x: 1, y: 2}, {x: 2, y: 2},
						{x: 1, y: 2},
					},
				},
			},
			show: true,
			expected: func() [][]bool {
				field := matrixInit(fieldHeight, fieldWidth)
				field[1][1] = true
				field[1][2] = true
				field[2][1] = true
				field[2][2] = true
				return field
			}(),
			description: "Должен отобразить врагов на поле",
		},
		{
			name: "Hide enemies on field with existing content",
			enemies: []car{
				{
					coords: [carSize]coord{
						{x: 3, y: 3}, {x: 4, y: 3},
						{x: 3, y: 4}, {x: 4, y: 4},
						{x: 3, y: 4}, {x: 4, y: 4},
						{x: 3, y: 4},
					},
				},
			},
			show: false,
			expected: func() [][]bool {
				field := matrixInit(fieldHeight, fieldWidth)
				return field
			}(),
			description: "Должен скрыть врагов с поля",
		},
		{
			name: "Multiple enemies show",
			enemies: []car{
				{
					coords: [carSize]coord{
						{x: 1, y: 1}, {x: 2, y: 1},
						{x: 1, y: 2}, {x: 2, y: 2},
						{x: 1, y: 2}, {x: 2, y: 2},
						{x: 1, y: 2},
					},
				},
				{
					coords: [carSize]coord{
						{x: 5, y: 5}, {x: 6, y: 5},
						{x: 5, y: 6}, {x: 6, y: 6},
						{x: 5, y: 6}, {x: 6, y: 6},
						{x: 5, y: 6},
					},
				},
			},
			show: true,
			expected: func() [][]bool {
				field := matrixInit(fieldHeight, fieldWidth)
				field[1][1] = true
				field[1][2] = true
				field[2][1] = true
				field[2][2] = true
				field[5][5] = true
				field[5][6] = true
				field[6][5] = true
				field[6][6] = true
				return field
			}(),
			description: "Должен отобразить нескольких врагов",
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			s.game.state.Field = matrixInit(fieldHeight, fieldWidth)
			s.game.enemies = tt.enemies

			s.game.renderEnemiesOnField(tt.show)

			for i := 0; i < fieldHeight; i++ {
				for j := 0; j < fieldWidth; j++ {
					assert.Equal(t, tt.expected[i][j], s.game.state.Field[i][j],
						"Несоответствие в клетке [%d][%d] для теста '%s'", i, j, tt.name)
				}
			}
		})
	}
}

func (s *RaceGameTestSuite) TestTimerFunctionsSimple() {
	t := s.T()

	t.Run("Simple startTimer test", func(t *testing.T) {
		game := createTestRaceGame()

		game.gameTimer = gametimer.New(10000)

		assert.NotPanics(t, func() {
			game.startTimer()
		})

		game.gameTimer.Stop()
	})

	t.Run("Simple stopTimer test", func(t *testing.T) {
		game := createTestRaceGame()

		game.gameTimer = gametimer.New(10000)
		game.gameTimer.Start()

		assert.NotPanics(t, func() {
			game.stopTimer()
		})
	})

	t.Run("StopTimer with nil timer", func(t *testing.T) {
		game := createTestRaceGame()
		game.gameTimer = nil

		assert.NotPanics(t, func() {
			game.stopTimer()
		})
	})
}

func (s *RaceGameTestSuite) TestShiftEnemiesSimple() {
	t := s.T()

	game := createTestRaceGame()

	game.enemies = []car{
		{
			coords: [carSize]coord{
				{x: 1, y: 1}, {x: 2, y: 1},
				{x: 1, y: 2}, {x: 2, y: 2},
				{x: 1, y: 2}, {x: 2, y: 2},
				{x: 1, y: 2},
			},
		},
	}

	assert.NotPanics(t, func() {
		game.shiftEnemies()
	})
}
