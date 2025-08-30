import { applyRootStyles } from './src/utils.js';
import { GameBoard } from './src/game-board.js';
import { rootStyles, keyCodes, GAME_BOARD_HEIGHT, GAME_BOARD_WIDTH, NEXT_BOARD_WIDTH, NEXT_BOARD_HEIGHT } from './src/config.js';

applyRootStyles(rootStyles);
const gameBoard = new GameBoard(document.querySelector('#game-board'), GAME_BOARD_WIDTH, GAME_BOARD_HEIGHT);
const nextBoard = new GameBoard(document.querySelector('#next-board'), NEXT_BOARD_WIDTH, NEXT_BOARD_HEIGHT);

const ACTIONS = {
    START: 0,
    PAUSE: 1,
    TERMINATE: 2,
    LEFT: 3,
    RIGHT: 4,
    UP: 5,
    DOWN: 6,
    ACTION: 7
};

function fetchState() {
    return fetch('/api/state')
        .then(function(response) {
            if (!response.ok) {
                throw new Error('HTTP error! status: ' + response.status);
            }
            return response.json();
        })
        .catch(function(error) {
            console.error('Error fetching state:', error);
            return null;
        });
}

function fetchGamesList() {
    return fetch('/api/games')
        .then(function(response) {
            if (!response.ok) {
                throw new Error('HTTP error! status: ' + response.status);
            }
            return response.json();
        })
        .then(function(data) {
            return data.games;
        })
        .catch(function(error) {
            console.error('Error fetching games list:', error);
            return [];
        });
}

fetchGamesList().then(renderGamesList);

function renderField(board, field) {
    for (let y = 0; y < board.height; y++) {
        for (let x = 0; x < board.width; x++) {
            if (field[y][x]) {
                board.enableTile(x, y);
            } else {
                board.disableTile(x, y);
            }
        }
    }
}

let gameLoopInterval = null;
let isGameRunning = false;

function startGameLoop() {
    if (gameLoopInterval) {
        clearInterval(gameLoopInterval);
        gameLoopInterval = null;
    }

    isGameRunning = true;

    function gameLoopTick() {
        if (!isGameRunning) return;

        fetchState().then(function(state) {
            if (!isGameRunning) return;

            if (!state) {
                stopGameLoop();
                return;
            }

            if (state.level === -1) {
                stopGameLoop();
                fetchGamesList().then(renderGamesList);
                return;
            }

            renderField(gameBoard, state.field);
            renderField(nextBoard, state.next);
            document.getElementById('score-value').textContent = state.score;
            document.getElementById('high-score-value').textContent = state.high_score;
            document.getElementById('level-value').textContent = state.level;
            document.getElementById('speed-value').textContent = state.speed;
            document.getElementById('pause-value').textContent = state.pause;
        });
    }

    gameLoopTick();
    
    gameLoopInterval = setInterval(gameLoopTick, 100);
}

function stopGameLoop() {
    isGameRunning = false;
    if (gameLoopInterval) {
        clearInterval(gameLoopInterval);
        gameLoopInterval = null;
    }
    showGamesList();
}

function sendAction(action, hold) {
    fetch('/api/actions', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({action_id: action, hold: hold})
    }).catch(function(error) {
        console.error('Error sending action:', error);
    });
}

document.addEventListener('keydown', function (event) {
    if (keyCodes.start.includes(event.code)) sendAction(ACTIONS.START, false);
    if (keyCodes.pause.includes(event.code)) sendAction(ACTIONS.PAUSE, false);
    if (keyCodes.terminate.includes(event.code)) sendAction(ACTIONS.TERMINATE, false);
    if (keyCodes.left.includes(event.code)) sendAction(ACTIONS.LEFT, false);
    if (keyCodes.right.includes(event.code)) sendAction(ACTIONS.RIGHT, false);
    if (keyCodes.up.includes(event.code)) sendAction(ACTIONS.UP, false);
    if (keyCodes.down.includes(event.code)) sendAction(ACTIONS.DOWN, false); 
    if (keyCodes.action.includes(event.code)) sendAction(ACTIONS.ACTION, false);
});

function renderGamesList(games) {
    showGamesList();

    const $list = document.getElementById('games-list');
    $list.innerHTML = '';

    games.forEach(function(game) {
        const btn = document.createElement('button');
        btn.textContent = game.name;
        btn.onclick = function() {
            selectGame(game.id);
        };
        $list.appendChild(btn);
    });
}

function selectGame(gameId) {
    if (gameLoopInterval) {
        clearInterval(gameLoopInterval);
        gameLoopInterval = null;
    }
    isGameRunning = false;

    fetch('/api/games/' + gameId, {
        method: 'POST'
    })
    .then(function(response) {
        if (!response.ok) {
            return response.json().then(function(data) {
                alert('Ошибка: ' + data.message);
                throw new Error(data.message);
            });
        }
        showGameUI();
        startGameLoop();
        return null;
    })
    .catch(function(error) {
        console.error('Ошибка при выборе игры:', error);
        fetchGamesList().then(renderGamesList);
    });
}

function showGamesList() {
    document.getElementById('game-board').style.display = 'none';
    document.getElementById('next-board').style.display = 'none';
    document.getElementById('side-panel').style.display = 'none';
    document.getElementById('games-list').style.display = '';
}

function showGameUI() {
    document.getElementById('games-list').style.display = 'none';
    document.getElementById('game-board').style.display = '';
    document.getElementById('next-board').style.display = '';
    document.getElementById('side-panel').style.display = '';
}