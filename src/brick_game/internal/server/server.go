package server

import (
	gamefabric "brick_game/internal/game_fabric"
	gamespec "brick_game/internal/game_spec"
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"
)

type GamesList struct {
	Games []GameInfo `json:"games"` // Список игр
}

type GameInfo struct {
	ID   int    `json:"id"`   // Идентификатор игры
	Name string `json:"name"` // Название игры
}

type UserAction struct {
	ActionID int  `json:"action_id"` // Идентификатор действия
	Hold     bool `json:"hold"`      // Флаг, отвечающий за зажатие кнопки
}

type GameState struct {
	Field     [][]bool `json:"field"`      // Матрица игрового поля
	Next      [][]bool `json:"next"`       // Поле доп. информации
	Score     int      `json:"score"`      // Текущее количество очков
	HighScore int      `json:"high_score"` // Максимальное количество очков
	Level     int      `json:"level"`      // Поле доп. информации
	Speed     int      `json:"speed"`      // Поле доп. информации
	Pause     bool     `json:"pause"`      // Поле доп. информации
}

type ErrorMessage struct {
	Message string `json:"message"` // Сообщение об ошибке
}

type GameServer struct {
	currentGame gamespec.Game
	gameMutex   sync.Mutex
}

func (s *GameServer) Start() {
	http.Handle("/", http.FileServer(http.Dir("../web")))
	http.HandleFunc("/api/games", s.handleGetGames)
	http.HandleFunc("/api/games/", s.handlePostGame)
	http.HandleFunc("/api/state", s.handleGetState)
	http.HandleFunc("/api/actions", s.handlePostAction)

	httpServer := &http.Server{
		Addr:    ":8080",
		Handler: nil,
	}

	stop := make(chan os.Signal, 1)
	signal.Notify(stop, os.Interrupt, syscall.SIGTERM)

	go func() {
		fmt.Println("Server started at http://localhost:8080")
		fmt.Println("API:      http://localhost:8080/api/")
		if err := httpServer.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("Server error: %v", err)
		}
	}()

	<-stop
	log.Println("Shutting down server...")

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	if err := httpServer.Shutdown(ctx); err != nil {
		log.Printf("Server shutdown error: %v", err)
	}

	log.Println("Server stopped")
}

// Обработчик GET /games
func (s *GameServer) handleGetGames(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Method not allowed")
		return
	}

	if s.currentGame != nil {
		s.currentGame = nil
	}

	rawList := gamefabric.GetGamesList()

	gamesList := make([]GameInfo, len(rawList))
	for i, game := range rawList {
		gamesList[i] = GameInfo{
			ID:   game.ID,
			Name: game.Name,
		}
	}
	response := GamesList{
		Games: gamesList,
	}

	if err := SendJSONResponse(w, http.StatusOK, response); err != nil {
		SendErrorResponse(w, http.StatusInternalServerError, "Failed to encode response")
		return
	}
}

// Обработчик POST /games/{gameId}
func (s *GameServer) handlePostGame(w http.ResponseWriter, r *http.Request) {
	var err error

	if r.Method != http.MethodPost {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Method not allowed")
		return
	}

	parts := strings.Split(r.URL.Path, "/")
	if len(parts) < 4 {
		SendErrorResponse(w, http.StatusBadRequest, "Game ID not provided")
		return
	}

	gameId, err := strconv.Atoi(parts[3])
	if err != nil {
		SendErrorResponse(w, http.StatusBadRequest, "Invalid game ID format")
		return
	}

	s.gameMutex.Lock()
	defer s.gameMutex.Unlock()

	if s.currentGame != nil {
		SendErrorResponse(w, http.StatusConflict, "Another game has been started")
		return
	}

	s.currentGame, err = gamefabric.CreateGame(gameId)
	if err != nil {
		SendErrorResponse(w, http.StatusNotFound, "Game not found")
		return
	}

	w.WriteHeader(http.StatusOK)
}

// Обработчик GET /state
func (s *GameServer) handleGetState(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Method not allowed")
		return
	}

	s.gameMutex.Lock()
	defer s.gameMutex.Unlock()

	if s.currentGame == nil {
		SendErrorResponse(w, http.StatusBadRequest, "Game not selected")
		return
	}

	internalState := s.currentGame.UpdateCurrentState()
	gameState := GameState{
		Field:     internalState.Field,
		Next:      internalState.Next,
		Score:     internalState.Score,
		HighScore: internalState.HighScore,
		Level:     internalState.Level,
		Speed:     internalState.Speed,
		Pause:     internalState.Pause,
	}

	if err := SendJSONResponse(w, http.StatusOK, gameState); err != nil {
		SendErrorResponse(w, http.StatusInternalServerError, "Failed to encode game state")
	}
}

// Обработчик POST /actions
func (s *GameServer) handlePostAction(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Method not allowed")
		return
	}

	var action UserAction
	if err := json.NewDecoder(r.Body).Decode(&action); err != nil {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Invalid JSON format")
		return
	}

	s.gameMutex.Lock()
	defer s.gameMutex.Unlock()

	if s.currentGame == nil {
		SendErrorResponse(w, http.StatusMethodNotAllowed, "Game not selected")
		return
	}

	var act gamespec.Action
	switch action.ActionID {
	case 0:
		act = gamespec.Start
	case 1:
		act = gamespec.Pause
	case 2:
		act = gamespec.Terminate
	case 3:
		act = gamespec.Left
	case 4:
		act = gamespec.Right
	case 5:
		act = gamespec.Up
	case 6:
		act = gamespec.Down
	case 7:
		act = gamespec.ActionBtn
	default:
		SendErrorResponse(w, http.StatusBadRequest, "Invalid action ID")
		return
	}

	s.currentGame.UserInput(act, action.Hold)

	w.WriteHeader(http.StatusOK)
}

func NewErrorMessage(message string) ErrorMessage {
	return ErrorMessage{Message: message}
}

// SendJSONResponse отправляет JSON ответ с правильным заголовком
func SendJSONResponse(w http.ResponseWriter, statusCode int, data any) error {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(statusCode)
	return json.NewEncoder(w).Encode(data)
}

// SendErrorResponse отправляет ошибку в формате ErrorMessage
func SendErrorResponse(w http.ResponseWriter, statusCode int, message string) error {
	errorMsg := NewErrorMessage(message)
	return SendJSONResponse(w, statusCode, errorMsg)
}
