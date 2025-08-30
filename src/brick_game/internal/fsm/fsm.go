package fsm

type State string

type Handler interface {
	Handle()
}

type FSM struct {
	currentState State
	handlers     map[State]Handler
}

func New(startState State) *FSM {
	return &FSM{
		currentState: startState,
		handlers:     make(map[State]Handler),
	}
}

func (fsm *FSM) SetState(state State) {
	fsm.currentState = state
}

func (fsm *FSM) GetState() State {
	return fsm.currentState
}

func (fsm *FSM) RegisterHandler(state State, handler Handler) {
	fsm.handlers[state] = handler
}

func (fsm *FSM) Update() {
	if handler, exists := fsm.handlers[fsm.currentState]; exists {
		handler.Handle()
	}
}

func (fsm *FSM) HasState(state State) bool {
	_, exists := fsm.handlers[state]
	return exists
}
