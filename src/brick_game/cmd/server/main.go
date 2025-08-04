package main

import (
	"brick_game/internal/games"
	"brick_game/internal/spec"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strings"
)

type GameServer struct {
	currentGame spec.Game // Текущая выбранная игра
}

type UserAction struct {
	Action string `json:"action"`
	Hold   bool   `json:"hold"`
}

func (s *GameServer) handleAction(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	var action UserAction
	if err := json.NewDecoder(r.Body).Decode(&action); err != nil {
		log.Printf("Ошибка декодирования JSON: %v", err)
		http.Error(w, `{"message":"Bad request"}`, http.StatusBadRequest)
		return
	}

	if s.currentGame == nil {
		log.Printf("Попытка отправить действие без выбранной игры")
		http.Error(w, `{"message":"Game not selected"}`, http.StatusBadRequest)
		return
	}

	var act spec.Action
	switch action.Action {
	case "Left":
		act = spec.Left
	case "Right":
		act = spec.Right
	case "Up":
		act = spec.Up
	case "Down":
		act = spec.Down
	case "Pause":
		act = spec.Pause
	case "Action":
		act = spec.ActionBtn
	case "Terminate":
		act = spec.Terminate
	case "Start":
		act = spec.Start
	default:
		http.Error(w, `{"message":"Invalid action"}`, http.StatusBadRequest)
		return
	}
	// fmt.Println("action: ", act, action.Hold)
	s.currentGame.UserInput(act, action.Hold)

	// C.userInput(C.UserAction_t(act), C.bool(action.Hold)) // CGO
	w.WriteHeader(http.StatusOK)
}

func (s *GameServer) handleState(w http.ResponseWriter, r *http.Request) {
	if s.currentGame == nil {
		w.WriteHeader(http.StatusBadRequest)
		json.NewEncoder(w).Encode(map[string]string{"message": "Game not selected"})
		return
	}

	state := s.currentGame.UpdateCurrentState()

	// //Получаем состояние из C
	// cGameInfo := C.updateCurrentState()
	// //fmt.Println("state: ", cGameInfo)
	// //Конвертируем в Go структуру
	// state := convertCGameInfoToGoState(cGameInfo)
	//fmt.Println("state: ", state)

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(state)
}

// Обработчик GET /games
func (s *GameServer) handleGetGame(w http.ResponseWriter, r *http.Request) {
	gamesList := games.GetGamesList()
	json.NewEncoder(w).Encode(map[string]any{"games": gamesList})
}

// Обработчик POST /games/{gameId}
func (s *GameServer) handleSelectGame(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	parts := strings.Split(r.URL.Path, "/")
	if len(parts) < 3 {
		http.Error(w, `{"message":"Game ID not provided"}`, http.StatusBadRequest)
		return
	}
	gameId := parts[2]

	var err error
	s.currentGame, err = games.CreateGame(gameId)
	if err != nil {
		w.WriteHeader(http.StatusNotFound)
		json.NewEncoder(w).Encode(map[string]string{"message": "Game not found"})
		return
	}

	w.WriteHeader(http.StatusOK)
}

func main() {
	server := &GameServer{}
	http.Handle("/", http.FileServer(http.Dir("../gui/web")))
	http.HandleFunc("/games", server.handleGetGame)
	http.HandleFunc("/games/", server.handleSelectGame)
	http.HandleFunc("/actions", server.handleAction)
	http.HandleFunc("/state", server.handleState)

	fmt.Println("Server started at :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
