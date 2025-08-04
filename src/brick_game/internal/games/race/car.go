package race

const (
	carSize = 7
)

type coord struct {
	y, x int
}

type car struct {
	isRight bool
	coords  [carSize]coord
}

func newCar(isRight bool, coords [carSize]coord) car {
	return car{
		isRight: isRight,
		coords:  coords,
	}
}

func (c *car) moveDown() {
	for i := range c.coords {
		c.coords[i].y++
	}
}

func (c *car) isOutOfField(fieldHeight int) bool {
	head := c.coords[carSize-1]
	return head.y >= fieldHeight
}

func (c *car) isCollidingWith(other *car) bool {
	if c.isRight != other.isRight {
		return false
	}
	head := other.coords[carSize-1]
	collideZone := fieldHeight - 8 // 8 - size of two cars (4 + 4)
	return head.y > collideZone
}

var (
	leftPlayerCoords = [carSize]coord{
		{16, 3}, {17, 2}, {17, 3},
		{17, 4}, {18, 3}, {19, 2},
		{19, 4}}
	rightPlayerCoords = [carSize]coord{
		{16, 6}, {17, 5}, {17, 6},
		{17, 7}, {18, 6}, {19, 5},
		{19, 7}}
	rightEnemyCoords = [carSize]coord{
		{-1, 5}, {-1, 7}, {-2, 6},
		{-3, 6}, {-3, 5}, {-3, 7},
		{-4, 6}}
	leftEnemyCoords = [carSize]coord{
		{-1, 2}, {-1, 4}, {-2, 3},
		{-3, 3}, {-3, 2}, {-3, 4},
		{-4, 3}}
)

var playerCarPositions = struct {
	left  car
	right car
}{
	left:  newCar(false, leftPlayerCoords),
	right: newCar(true, rightPlayerCoords),
}

var enemyCarPositions = struct {
	left  car
	right car
}{
	left:  newCar(false, leftEnemyCoords),
	right: newCar(true, rightEnemyCoords),
}
