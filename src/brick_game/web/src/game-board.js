export class GameBoard {
    constructor($gameBoard, width, height) {
        this.element = $gameBoard;
        this.width = width;
        this.height = height;
        this.tiles = [];
        
        for (let y = 0; y < this.height; ++y) {
            for (let x = 0; x < this.width; ++x) {
                const $tile = document.createElement('div');
                $tile.classList.add('tile');
                $tile.id = `position-${x}-${y}`;
                this.tiles.push($tile);
                this.element.append($tile);
            }
        }
    }

    getTile(x, y) {
        return this.tiles[y * this.width + x];
    }

    enableTile(x, y) {
        this.getTile(x, y).classList.add('active');
    }

    disableTile(x, y) {
        this.getTile(x, y).classList.remove('active');
    }
}
