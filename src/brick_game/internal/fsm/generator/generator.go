package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"text/template"
)

type StateData struct {
	StateName   string
	HandlerName string
}

type FSMData struct {
	PackageName string
	States      []StateData
	StartState  string
	EntityName  string
}

const fsmTemplate = `package {{.PackageName}}

import (
	"brick_game/internal/fsm"
)

// State constants for {{.PackageName}}
const (
{{range .States}}	{{.StateName}} fsm.State = "{{.StateName}}"
{{end}})

// State handlers for {{.PackageName}}
{{range .States}}type {{.HandlerName}}Handler struct {
	{{$.EntityName}} *{{$.PackageName}}{{$.EntityName}}
}

func (h *{{.HandlerName}}Handler) Handle() {
	// TODO: Add {{.StateName}} state logic
}

{{end}}`

func GenerateFSMFile() {
	fmt.Println("=== FSM Generator ===")

	projectPath := getInput("Project path: ")
	packageName := getInput("Package name: ")
	entityName := getInput("Entity name (e.g., game, app): ")

	states := getStates()

	fsmData := FSMData{
		PackageName: packageName,
		States:      states,
		StartState:  states[0].StateName,
		EntityName:  entityName,
	}

	generateFSMFile(projectPath, packageName, fsmData)
	fmt.Println("FSM file created successfully!")
}

func getInput(prompt string) string {
	fmt.Print(prompt)
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	return strings.TrimSpace(scanner.Text())
}

func getStates() []StateData {
	var states []StateData

	fmt.Println("\nEnter states (empty to finish):")
	for {
		stateName := getInput("State: ")
		if stateName == "" {
			break
		}

		states = append(states, StateData{
			StateName:   stateName,
			HandlerName: toCamelCase(stateName),
		})
	}

	if len(states) == 0 {
		fmt.Println("Error: at least one state required")
		os.Exit(1)
	}

	return states
}

func toCamelCase(s string) string {
	words := strings.Fields(s)
	if len(words) == 0 {
		return ""
	}

	result := strings.ToLower(words[0])
	for i := 1; i < len(words); i++ {
		word := strings.ToLower(words[i])
		if len(word) > 0 {
			result += strings.ToUpper(word[:1]) + word[1:]
		}
	}

	return result
}

func generateFSMFile(projectPath, packageName string, fsmData FSMData) {
	packageDir := filepath.Join(projectPath, packageName)
	if err := os.MkdirAll(packageDir, 0755); err != nil {
		fmt.Printf("Error creating directory: %v\n", err)
		os.Exit(1)
	}

	filePath := filepath.Join(packageDir, packageName+"Fsm.go")
	file, err := os.Create(filePath)
	if err != nil {
		fmt.Printf("Error creating file: %v\n", err)
		os.Exit(1)
	}
	defer file.Close()

	tmpl := template.Must(template.New("fsm").Parse(fsmTemplate))
	if err := tmpl.Execute(file, fsmData); err != nil {
		fmt.Printf("Error writing file: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("File created: %s\n", filePath)
}
