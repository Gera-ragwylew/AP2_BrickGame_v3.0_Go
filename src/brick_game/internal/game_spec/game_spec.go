package gamespec

type Action int

const (
	Start Action = iota
	Pause
	Terminate
	Left
	Right
	Up
	Down
	ActionBtn
)

type State struct {
	Field     [][]bool
	Next      [][]bool
	Score     int
	HighScore int
	Level     int
	Speed     int
	Pause     bool
}

type Game interface {
	UserInput(action Action, hold bool)
	UpdateCurrentState() State
}
