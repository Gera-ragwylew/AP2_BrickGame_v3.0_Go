package main

import (
	"brick_game/internal/server"
)

func main() {
	server := &server.GameServer{}
	server.Start()
}
