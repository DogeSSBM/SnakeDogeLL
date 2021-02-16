#include "Includes.h"

typedef struct Snake{
	Coord pos;
	struct Snake *next;
}Snake;

void drawSnake(Snake *snake, const uint scale)
{
	setColor(GREEN);
	Snake *current = snake;
	do{
		const Coord pos = coordMul(current->pos, scale);
		fillSquareCoord(pos, scale);
		current = current->next;
	}while(current != NULL);
}

void drawApple(const Coord apple, const uint scale)
{
	setColor(RED);
	const Coord pos = coordMul(apple, scale);
	fillSquareCoord(pos, scale);
}

bool inSnake(const Coord pos, Snake *snake)
{
	Snake *current = snake;
	do{
		if(coordSame(pos, current->pos))
			return true;
		current = current->next;
	}while(current != NULL);
	return false;
}

Coord placeApple(Snake *snake, const Length grid)
{
	Coord ret;
	do{
		ret = (Coord){rand()%grid.x, rand()%grid.y};
	}while(inSnake(ret, snake));
	return ret;
}

void readPressedDir(bool *pressedDir)
{
	pressedDir[0]|=keyPressed(SDL_SCANCODE_W)||keyPressed(SDL_SCANCODE_UP);
	pressedDir[1]|=keyPressed(SDL_SCANCODE_D)||keyPressed(SDL_SCANCODE_RIGHT);
	pressedDir[2]|=keyPressed(SDL_SCANCODE_S)||keyPressed(SDL_SCANCODE_DOWN);
	pressedDir[3]|=keyPressed(SDL_SCANCODE_A)||keyPressed(SDL_SCANCODE_LEFT);
}

Direction getDir(bool *pressedDir, const Direction current)
{
	Direction ret = current;
	for(uint i = 0; i < 4; i++){
		if(pressedDir[i] &&
		i != dirINV(current) &&
		!pressedDir[dirINV(i)])
			ret = i;
	}
	memset(pressedDir, 0, sizeof(bool)*4);
	return ret;
}

void loose(Snake *snake)
{
	if(snake != NULL){
		Snake *next = snake->next;
		free(snake);
		loose(next);
	}
	printf("You loose\n");
	exit(0);
}

Snake* cycle(Snake *snake, const Length grid, const Direction dir, bool grow)
{
	if(grow){
		Snake *ret = malloc(sizeof(Snake));
		ret->next = snake;
		ret->pos = coordShift(snake->pos, dir, 1);
		if(!inBound(ret->pos.x,0,grid.x) ||
		!inBound(ret->pos.y,0,grid.y) ||
		inSnake(ret->pos, ret->next))
			loose(snake);
		return ret;
	}

	Snake *current = snake;
	Snake *ret;
	while(current->next->next != NULL){
		current = current->next;
	}
	ret = current->next;
	current->next = NULL;

	ret->next = snake;
	ret->pos = coordShift(snake->pos, dir, 1);
	if(!inBound(ret->pos.x,0,grid.x) ||
	!inBound(ret->pos.y,0,grid.y) ||
	inSnake(ret->pos, ret->next))
		loose(snake);
	return ret;
}

int main(int argc, char const *argv[])
{
	const Length window = {800, 600};
	init(window);
	const uint scale = 40;
	const Length grid = coordDiv(window, scale);

	Snake *snake = malloc(sizeof(Snake));
	Direction dir = DIR_L;

	snake->pos = coordDiv(grid, 2);
	snake->next = malloc(sizeof(Snake));
	snake->next->pos = coordShift(snake->pos, dirINV(dir), 1);
	snake->next->next = NULL;

	Coord apple = placeApple(snake, grid);

	uint frameCount = 0;
	bool pressedDir[4] = {0};
	while(1){
		Ticks frameStart = getTicks();
		clear();

		readPressedDir(pressedDir);
		if(frameCount++ >= FPS/4){
			dir = getDir(pressedDir, dir);
			const bool grow = coordSame(
				apple,
				coordShift(snake->pos,dir,1)
			);
			if(grow)
				apple = placeApple(snake, grid);
			snake = cycle(snake, grid, dir, grow);
			frameCount = 0;
		}
		drawSnake(snake, scale);
		drawApple(apple, scale);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
