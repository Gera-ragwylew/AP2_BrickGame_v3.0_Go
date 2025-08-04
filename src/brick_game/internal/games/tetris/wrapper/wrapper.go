package wrapper

// #cgo LDFLAGS: -L../c_tetris -ltetris
// #include "../c_tetris/backend/backend.h"
// #include "../c_tetris/backend/fsm.h"
import "C"

import (
	"brick_game/internal/spec"
	"unsafe"
)

func UserInput(action spec.Action, hold bool) {
	C.userInput(C.UserAction_t(action), C.bool(hold))
}

func UpdateCurrentState() spec.State {
	return convertCGameInfoToGoState(C.updateCurrentState())
}

// export getIntArrayValue
func getIntArrayValue(array unsafe.Pointer, row, col int, rowSize, colSize int) int {
	if array == nil {
		return 0
	}

	// Получаем указатель на строку
	rowPtr := *(*unsafe.Pointer)(unsafe.Pointer(uintptr(array) + uintptr(row)*unsafe.Sizeof(unsafe.Pointer(nil))))
	if rowPtr == nil {
		return 0
	}

	// Проверяем границы
	if row < 0 || row >= rowSize || col < 0 || col >= colSize {
		return 0
	}

	// Получаем значение
	value := *(*C.int)(unsafe.Pointer(uintptr(rowPtr) + uintptr(col)*unsafe.Sizeof(C.int(0))))
	return int(value)
}

// Функция конвертации C GameInfo_t в Go State
func convertCGameInfoToGoState(cGameInfo C.GameInfo_t) spec.State {
	// Конвертируем field (int** в [][]bool)
	field := make([][]bool, C.WINDOW_HEIGHT)
	for i := 0; i < int(C.WINDOW_HEIGHT); i++ {
		field[i] = make([]bool, C.WINDOW_WIDTH)
		for j := 0; j < int(C.WINDOW_WIDTH); j++ {
			value := getIntArrayValue(unsafe.Pointer(cGameInfo.field), i, j, int(C.WINDOW_HEIGHT), int(C.WINDOW_WIDTH))
			field[i][j] = value != 0
		}
	}

	// Конвертируем next (int** в [][]bool)
	next := make([][]bool, C.NEXT_SIZE)
	for i := 0; i < int(C.NEXT_SIZE); i++ {
		next[i] = make([]bool, C.NEXT_SIZE)
		for j := 0; j < int(C.NEXT_SIZE); j++ {
			value := getIntArrayValue(unsafe.Pointer(cGameInfo.next), i, j, int(C.NEXT_SIZE), int(C.NEXT_SIZE))
			next[i][j] = value != 0
		}
	}
	return spec.State{
		Field:     field,
		Next:      next,
		Score:     int(cGameInfo.score),
		HighScore: int(cGameInfo.high_score),
		Level:     int(cGameInfo.level),
		Speed:     int(cGameInfo.speed),
		Pause:     cGameInfo.pause != 0,
	}
}
