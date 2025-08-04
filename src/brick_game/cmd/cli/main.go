package main

// import (
// 	"brick_game/race"
// 	"brick_game/spec"
// 	"fmt"
// 	"log"
// 	"os"
// 	"time"

// 	"github.com/nsf/termbox-go"
// )

// func printMatrix(m [][]bool) {
// 	for i := range m {
// 		for _, v := range m[i] {
// 			if v {
// 				fmt.Printf("1 ")
// 			} else {
// 				fmt.Printf("0 ")
// 			}
// 		}
// 		fmt.Println()
// 	}
// 	fmt.Println()
// }

// func main() {
// 	err := termbox.Init()
// 	if err != nil {
// 		log.Fatal(err)
// 	}
// 	defer termbox.Close()
// 	termbox.SetInputMode(termbox.InputEsc)

// 	myRace := race.New()
// 	myState := myRace.UpdateCurrentState()

// 	userAction := spec.Start
// 	eventCh := make(chan termbox.Event)
// 	go func() {
// 		for {
// 			eventCh <- termbox.PollEvent()
// 		}
// 	}()

// 	for userAction != spec.Terminate && myState.Level != -1 {
// 		select {
// 		case ev := <-eventCh:
// 			userAction = handleInput(ev)
// 			myRace.UserInput(userAction, false)
// 			userAction = spec.Start
// 		default:
// 		}

// 		myState = myRace.UpdateCurrentState()
// 		printMatrix(myState.Field)
// 		time.Sleep(10 * time.Millisecond)
// 	}
// }

// func handleInput(ev termbox.Event) spec.Action {
// 	var userAction spec.Action
// 	switch ev.Key {
// 	case termbox.KeyArrowUp:
// 		userAction = spec.Up
// 	case termbox.KeyArrowDown:
// 		userAction = spec.Down
// 	case termbox.KeyArrowLeft:
// 		userAction = spec.Left
// 	case termbox.KeyArrowRight:
// 		userAction = spec.Right
// 	case termbox.KeyEsc:
// 		os.Exit(0)
// 		userAction = spec.Terminate
// 	default:
// 		switch ev.Ch {
// 		case 'p', 'P':
// 			userAction = spec.Pause
// 		case 's', 'S':
// 			userAction = spec.Start
// 		}
// 	}
// 	return userAction
// }
