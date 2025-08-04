export const GAME_BOARD_WIDTH = 10;
export const GAME_BOARD_HEIGHT = 20;
export const NEXT_BOARD_WIDTH = 4;
export const NEXT_BOARD_HEIGHT = 4;

export const rootStyles = {
    '--tile-size': '25px', // Размер тайлов для основного поля
    '--next-tile-size': '15px', // Размер тайлов для поля next
    '--tile-color': '#eee',
    '--tile-active-color': '#222',
    '--game-board-width': GAME_BOARD_WIDTH,
    '--game-board-height': GAME_BOARD_HEIGHT,
    '--game-board-gap': '2px',
    '--game-board-background': '#333',
    '--next-board-width': NEXT_BOARD_WIDTH,
    '--next-board-height': NEXT_BOARD_HEIGHT,
};

export const keyCodes = {
    up: ['ArrowUp'],
    right: ['ArrowRight'],
    down: ['ArrowDown'],
    left: ['ArrowLeft'],
    pause: ['Space', 'KeyP'],
    action: ['Enter', 'KeyA'],
    terminate: ['Escape', 'KeyQ'],
    start: ['KeyS'],
};