package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"text/template"
)

// Структура для хранения данных о состояниях
type StateData struct {
	StateName   string
	HandlerName string
}

// Структура для хранения всех данных FSM
type FSMData struct {
	PackageName string
	States      []StateData
	StartState  string
	EntityName  string
}

// Шаблон для генерации FSM файла
const fsmTemplate = `package {{.PackageName}}

import (
	"brick_game/internal/fsm"
)

// Константы состояний для {{.PackageName}}
const (
{{range .States}}	{{.StateName}} fsm.State = "{{.StateName}}"
{{end}})

// Обработчики состояний для {{.PackageName}}
{{range .States}}type {{.HandlerName}}Handler struct {
	{{$.EntityName}} *{{$.PackageName}}{{$.EntityName}}
}

func (h *{{.HandlerName}}Handler) Handle() {
	// TODO: Добавить логику обработки состояния {{.StateName}}
	// Примеры действий:
	// - h.{{$.EntityName}}.someMethod()
	// - h.{{$.EntityName}}.fsm.SetState(NextState)
	// - h.{{$.EntityName}}.updateSomething()
	// - h.{{$.EntityName}}.checkConditions()
}

{{end}}`

// GenerateFSMFile создает FSM файл на основе введенных данных
func GenerateFSMFile() {
	fmt.Println("=== Генератор FSM структур ===")

	// Получаем путь к проекту
	projectPath := getInput("Введите путь к проекту: ")

	// Получаем название пакета
	packageName := getInput("Введите название пакета: ")

	// Получаем название сущности (entity)
	entityName := getInput("Введите название сущности (например, game, app, service): ")

	// Получаем состояния
	states := getStates()

	// Создаем данные для шаблона
	fsmData := FSMData{
		PackageName: packageName,
		States:      states,
		StartState:  states[0].StateName,
		EntityName:  entityName,
	}

	// Генерируем файл
	generateFSMFile(projectPath, packageName, fsmData)

	fmt.Println("FSM файл успешно создан!")
}

func getInput(prompt string) string {
	fmt.Print(prompt)
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	return strings.TrimSpace(scanner.Text())
}

func getStates() []StateData {
	var states []StateData

	fmt.Println("\nВведите состояния (пустая строка для завершения):")

	for {
		stateName := getInput("Состояние: ")
		if stateName == "" {
			break
		}

		// Преобразуем название состояния в camelCase для имени обработчика
		handlerName := toCamelCase(stateName)

		states = append(states, StateData{
			StateName:   stateName,
			HandlerName: handlerName,
		})
	}

	if len(states) == 0 {
		fmt.Println("Ошибка: должно быть указано хотя бы одно состояние")
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
	// Создаем директорию если не существует
	packageDir := filepath.Join(projectPath, packageName)
	if err := os.MkdirAll(packageDir, 0755); err != nil {
		fmt.Printf("Ошибка создания директории: %v\n", err)
		os.Exit(1)
	}

	// Создаем имя файла
	fileName := packageName + "Fsm.go"
	filePath := filepath.Join(packageDir, fileName)

	// Создаем файл
	file, err := os.Create(filePath)
	if err != nil {
		fmt.Printf("Ошибка создания файла: %v\n", err)
		os.Exit(1)
	}
	defer file.Close()

	// Парсим и выполняем шаблон
	tmpl, err := template.New("fsm").Parse(fsmTemplate)
	if err != nil {
		fmt.Printf("Ошибка парсинга шаблона: %v\n", err)
		os.Exit(1)
	}

	// Записываем результат в файл
	if err := tmpl.Execute(file, fsmData); err != nil {
		fmt.Printf("Ошибка записи в файл: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("Файл создан: %s\n", filePath)
}
