package gametimer

import "time"

type GameTimer struct {
	ticker   *time.Ticker
	interval time.Duration
	tickChan <-chan time.Time
}

func New(intervalMs int) *GameTimer {
	if intervalMs <= 0 {
		intervalMs = 100 // значение по умолчанию
	}

	return &GameTimer{
		interval: time.Duration(intervalMs) * time.Millisecond,
	}
}

func (gt *GameTimer) Start() {
	if gt.ticker != nil {
		gt.ticker.Stop()
	}
	gt.ticker = time.NewTicker(gt.interval)
	gt.tickChan = gt.ticker.C
}

func (gt *GameTimer) Stop() {
	if gt.ticker != nil {
		gt.ticker.Stop()
		gt.ticker = nil
		gt.tickChan = nil
	}
}

func (gt *GameTimer) Reset(intervalMs int) {
	if intervalMs <= 0 {
		return
	}

	gt.interval = time.Duration(intervalMs) * time.Millisecond
	if gt.ticker != nil {
		gt.ticker.Reset(gt.interval)
	}
}

func (gt *GameTimer) FastReset() {
	if gt.ticker != nil {
		gt.ticker.Reset(1 * time.Millisecond)
	}
}

func (gt *GameTimer) TickChan() <-chan time.Time {
	if gt.tickChan == nil {
		// Возвращаем закрытый канал, если таймер не запущен
		closedChan := make(chan time.Time)
		close(closedChan)
		return closedChan
	}
	return gt.tickChan
}

// IsRunning проверяет, запущен ли таймер
func (gt *GameTimer) IsRunning() bool {
	return gt.ticker != nil
}
