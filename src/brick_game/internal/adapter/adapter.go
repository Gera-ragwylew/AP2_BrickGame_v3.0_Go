package adapter

import (
	"bufio"
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"syscall"
	"time"

	gamespec "brick_game/internal/game_spec"
)

const (
	StartCmd     = "Start"
	PauseCmd     = "Pause"
	TerminateCmd = "Terminate"
	LeftCmd      = "Left"
	RightCmd     = "Right"
	UpCmd        = "Up"
	DownCmd      = "Down"
	ActionCmd    = "Action"

	HoldFalse = "0"
	HoldTrue  = "1"
)

type adapter struct {
	cmd    *exec.Cmd
	stdin  io.WriteCloser
	stdout io.ReadCloser
	state  gamespec.State
	action gamespec.Action
}

type gameState struct {
	Score     int     `json:"score"`
	HighScore int     `json:"high_score"`
	Level     int     `json:"level"`
	Speed     int     `json:"speed"`
	Pause     int     `json:"pause"`
	Field     [][]int `json:"field"`
	Next      [][]int `json:"next"`
}

func New(name string) gamespec.Game {
	var processPath string
	switch name {
	case "tetris":
		processPath = "./bin/tetris_process"
	case "snake":
		processPath = "./bin/snake_process"
	}

	cmd := exec.Command(processPath)

	stdin, err := cmd.StdinPipe()
	if err != nil {
		log.Fatal("Error stdin:", err)
	}

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatal("Error stdout:", err)
	}

	if err := cmd.Start(); err != nil {
		log.Fatal("Error start:", err)
	}

	w := &adapter{
		cmd:    cmd,
		stdin:  stdin,
		stdout: stdout,
		state:  gamespec.State{},
		action: gamespec.Start,
	}

	return w
}

func (w *adapter) UserInput(action gamespec.Action, hold bool) {
	w.action = action
}

func (w *adapter) UpdateCurrentState() gamespec.State {
	actionStr, err := actionToString(w.action)
	if err != nil {
		log.Printf("Invalid action: %v", err)
		return w.state
	}

	_, err = io.WriteString(w.stdin, actionStr+" "+HoldFalse+"\n")
	if err != nil {
		log.Printf("Write error: %v", err)
		return w.state
	}

	reader := bufio.NewReader(w.stdout)
	line, err := reader.ReadBytes('\n')
	if err != nil {
		log.Printf("Read error: %v", err)
		return w.state
	}

	var gs gameState
	if err := json.Unmarshal(line, &gs); err != nil {
		log.Printf("Unmarshal error: %v", err)
		return w.state
	}

	convertedState := convertGameState(gs)
	w.state = convertedState

	if w.state.Level == -1 {
		w.close()
	}

	w.action = gamespec.Start

	return w.state
}

func convertGameState(gs gameState) gamespec.State {
	if gs.Field == nil || gs.Next == nil {
		log.Printf("Warning: Invalid game state - nil arrays")
		return gamespec.State{}
	}

	field := make([][]bool, len(gs.Field))
	for i, row := range gs.Field {
		if row == nil {
			log.Printf("Warning: Field row %d is nil", i)
			continue
		}
		field[i] = make([]bool, len(row))
		for j, val := range row {
			field[i][j] = val != 0
		}
	}

	next := make([][]bool, len(gs.Next))
	for i, row := range gs.Next {
		if row == nil {
			log.Printf("Warning: Next row %d is nil", i)
			continue
		}
		next[i] = make([]bool, len(row))
		for j, val := range row {
			next[i][j] = val != 0
		}
	}

	result := gamespec.State{
		Field:     field,
		Next:      next,
		Score:     gs.Score,
		HighScore: gs.HighScore,
		Level:     gs.Level,
		Speed:     gs.Speed,
		Pause:     gs.Pause != 0,
	}

	return result
}

func actionToString(a gamespec.Action) (string, error) {
	switch a {
	case gamespec.Start:
		return StartCmd, nil
	case gamespec.Pause:
		return PauseCmd, nil
	case gamespec.Terminate:
		return TerminateCmd, nil
	case gamespec.Left:
		return LeftCmd, nil
	case gamespec.Right:
		return RightCmd, nil
	case gamespec.Up:
		return UpCmd, nil
	case gamespec.Down:
		return DownCmd, nil
	case gamespec.ActionBtn:
		return ActionCmd, nil
	default:
		return "", errors.New("unknown action")
	}
}

func (w *adapter) close() {
	log.Printf("Closing process for %v", w.cmd.Path)

	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	var errors []error

	if err := w.stdin.Close(); err != nil {
		log.Printf("Error closing stdin: %v", err)
		errors = append(errors, err)
	}

	if w.cmd.Process != nil {
		if err := gracefulShutdown(ctx, w.cmd.Process); err != nil {
			log.Printf("Graceful shutdown failed: %v", err)
			errors = append(errors, err)

			if killErr := w.cmd.Process.Kill(); killErr != nil {
				log.Printf("Error killing process: %v", killErr)
				errors = append(errors, killErr)
			}
		}
	}

	if err := w.stdout.Close(); err != nil {
		log.Printf("Error closing stdout: %v", err)
		errors = append(errors, err)
	}

	if len(errors) > 0 {
		log.Printf("multiple errors during close: %v", errors)
	}
}

func gracefulShutdown(ctx context.Context, process *os.Process) error {
	if err := process.Signal(syscall.SIGTERM); err != nil {
		return fmt.Errorf("sending SIGTERM: %w", err)
	}

	done := make(chan error, 1)
	go func() {
		_, err := process.Wait()
		done <- err
	}()

	select {
	case <-ctx.Done():
		return ctx.Err()
	case err := <-done:
		return err
	}
}
