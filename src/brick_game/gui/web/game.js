import { applyRootStyles } from './src/utils.js';
import { GameBoard } from './src/game-board.js';
import { rootStyles, keyCodes, GAME_BOARD_HEIGHT, GAME_BOARD_WIDTH, NEXT_BOARD_WIDTH, NEXT_BOARD_HEIGHT } from './src/config.js';

applyRootStyles(rootStyles);
const gameBoard = new GameBoard(document.querySelector('#game-board'), GAME_BOARD_WIDTH, GAME_BOARD_HEIGHT);

// Создаем маленький board 5x5 для next piece
const nextBoard = new GameBoard(document.querySelector('#next-board'), NEXT_BOARD_WIDTH, NEXT_BOARD_HEIGHT);

// const $sidePanel = document.querySelector('#side-panel');

async function fetchState() {
    const res = await fetch('/state');
    return await res.json();
}

async function fetchGamesList() {
    const res = await fetch('/games');
    const data = await res.json();
    return data.games; // [{id: "1", name: "race"}, ...]
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

function startGameLoop() {
    gameLoopInterval = setInterval(async () => {
        const state = await fetchState();
        renderField(gameBoard, state.Field);
        renderField(nextBoard, state.Next);
        document.getElementById('score-value').textContent = state.Score;
        document.getElementById('high-score-value').textContent = state.HighScore;
        document.getElementById('level-value').textContent = state.Level;
        document.getElementById('speed-value').textContent = state.Speed;
        // document.getElementById('pause').textContent = state.Pause;

        // console.log(state.Level);
        if (state.Level === -1) {
            clearInterval(gameLoopInterval);
            gameLoopInterval = null;
            document.getElementById('game-board').style.display = 'none';
            document.getElementById('side-panel').style.display = 'none';
            document.getElementById('games-list').style.display = '';
            fetchGamesList().then(renderGamesList);
        }
    }, 20);
}

function sendAction(action, hold = false) {
    fetch('/actions', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({action, hold})
    });
}

document.addEventListener('keydown', function (event) {
    if (keyCodes.up.includes(event.code)) sendAction('Up');
    if (keyCodes.right.includes(event.code)) sendAction('Right');
    if (keyCodes.down.includes(event.code)) sendAction('Down'); 
    if (keyCodes.left.includes(event.code)) sendAction('Left');
    if (keyCodes.pause.includes(event.code)) sendAction('Pause');
    if (keyCodes.action.includes(event.code)) sendAction('Action');
    if (keyCodes.terminate.includes(event.code)) sendAction('Terminate');
    if (keyCodes.start.includes(event.code)) sendAction('Start');
});

function renderGamesList(games) {
    document.getElementById('game-board').style.display = 'none';
    document.getElementById('side-panel').style.display = 'none';

    const $list = document.getElementById('games-list');
    $list.innerHTML = ''; // Очистить контейнер

    games.forEach(game => {
        const btn = document.createElement('button');
        btn.textContent = game.Name; // Текст на кнопке
        btn.onclick = () => selectGame(game.ID); // Обработчик клика
        $list.appendChild(btn); // Добавить кнопку в контейнер
    });
}

function selectGame(gameId) {
    if (gameLoopInterval) {
        clearInterval(gameLoopInterval);
        gameLoopInterval = null;
    }
    console.log('Выбрана игра с id: ' + gameId);

    fetch(`/games/${gameId}`, {
        method: 'POST'
    })
    .then(response => {
        if (!response.ok) {
            return response.json().then(data => {
                alert('Ошибка: ' + (data.message || 'Не удалось выбрать игру'));
                throw new Error(data.message);
            });
        }
        const $gamesList = document.getElementById('games-list');
        if ($gamesList) {
            $gamesList.style.display = 'none';
        }

        document.getElementById('game-board').style.display = '';
        document.getElementById('next-board').style.display = '';
        document.getElementById('side-panel').style.display = '';
        startGameLoop();
    })
    .catch(error => {
        console.error('Ошибка при выборе игры:', error);
    });
}