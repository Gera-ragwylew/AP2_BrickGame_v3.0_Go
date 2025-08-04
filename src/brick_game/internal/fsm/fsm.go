package fsm

// State представляет состояние конечного автомата
type State string

// Handler интерфейс для обработчиков состояний
type Handler interface {
	Handle()
}

// FSM универсальный конечный автомат
type FSM struct {
	currentState State
	handlers     map[State]Handler
}

// New создает новый FSM
func New(startState State) *FSM {
	return &FSM{
		currentState: startState,
		handlers:     make(map[State]Handler),
	}
}

// SetState устанавливает текущее состояние
func (fsm *FSM) SetState(state State) {
	fsm.currentState = state
}

// GetState возвращает текущее состояние
func (fsm *FSM) GetState() State {
	return fsm.currentState
}

// RegisterHandler регистрирует обработчик для состояния
func (fsm *FSM) RegisterHandler(state State, handler Handler) {
	fsm.handlers[state] = handler
}

// Update выполняет обработку текущего состояния
func (fsm *FSM) Update() {
	if handler, exists := fsm.handlers[fsm.currentState]; exists {
		handler.Handle()
	}
}

// HasState проверяет, зарегистрировано ли состояние
func (fsm *FSM) HasState(state State) bool {
	_, exists := fsm.handlers[state]
	return exists
}
