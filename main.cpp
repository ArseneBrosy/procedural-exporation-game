#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

#pragma region CONSTANTS
const int DISPLAY_WIDTH = 1000;
const int DISPLAY_HEIGHT = 1000;
const int GRID_SIZE = 20;
const double WALL_VALUE_TOP = 0.5;
const double WALL_VALUE_BOTTOM = 0.5;
#pragma endregion

#pragma region VARIABLES
bool spaceDown = false;

SDL_Renderer* renderer;
double displayOffsetX, displayOffsetY, mulRatio, xCellSize, yCellSize;

vector<vector<int>> intersections;
#pragma endregion

#pragma region FUNCTIONS
int calculateIntersection(int x, int y) {
	return rand() % 2;
}

vector<vector<int>> generateIntersections() {
	vector<vector<int>> grid;
	for (int y = 0; y <= GRID_SIZE; y++) {
		vector<int> row;
		for (int x = 0; x <= GRID_SIZE; x++) {
			row.push_back(calculateIntersection(x, y));
		}
		grid.push_back(row);
	}
	return grid;
}

void drawLines(int x, int y) {
	// calculate the wall values
	const double wallValue1 = (WALL_VALUE_BOTTOM - WALL_VALUE_TOP) * (y / GRID_SIZE) + WALL_VALUE_TOP;
	const double wallValue2 = (WALL_VALUE_BOTTOM - WALL_VALUE_TOP) * ((y + 1) / GRID_SIZE) + WALL_VALUE_TOP;

	// calculate the case index (wich tile to place)
	const int a = intersections[y][x] > wallValue1 ? 1 : 0;
	const int b = intersections[y][x + 1] > wallValue1 ? 1 : 0;
	const int c = intersections[y + 1][x + 1] > wallValue2 ? 1 : 0;
	const int d = intersections[y + 1][x] > wallValue2 ? 1 : 0;
	const int caseIndex = a * 8 + b * 4 + c * 2 + d;

	// draw the line
	const int xZero = x * xCellSize * mulRatio + displayOffsetX;
	const int xHalf = xZero + xCellSize / 2 * mulRatio;
	const int xFull = xZero + xCellSize * mulRatio;
	const int yZero = y * xCellSize * mulRatio + displayOffsetY;
	const int yHalf = yZero + xCellSize / 2 * mulRatio;
	const int yFull = yZero + xCellSize * mulRatio;
	
	switch (caseIndex) {
	case 14:
	case 1:
		SDL_RenderDrawLine(renderer, xHalf, yFull, xZero, yHalf); break;
	case 13:
	case 2:
		SDL_RenderDrawLine(renderer, xFull, yHalf, xHalf, yFull); break;
	case 12:
	case 3:
		SDL_RenderDrawLine(renderer, xFull, yHalf, xZero, yHalf); break;
	case 11:
	case 4:
		SDL_RenderDrawLine(renderer, xHalf, yZero, xFull, yHalf); break;
	case 10:
	case 5:
		SDL_RenderDrawLine(renderer, xHalf, yZero, xFull, yHalf);
		SDL_RenderDrawLine(renderer, xHalf, yFull, xZero, yHalf); break;
	case 9:
	case 6:
		SDL_RenderDrawLine(renderer, xHalf, yZero, xHalf, yFull); break;
	case 8:
	case 7:
		SDL_RenderDrawLine(renderer, xHalf, yZero, xZero, yHalf); break;
	}
}

/*
function point(x, y, radius, color = "black") {
	ctx.fillStyle = color;
	ctx.beginPath();
	ctx.arc(x, y, radius, 0, 2 * Math.PI);
	ctx.fill();
}*/

void drawRectangle(double x, double y, double w, double h) {
	SDL_Rect rect;
	rect.x = static_cast<int>(x * mulRatio + displayOffsetX);
	rect.y = static_cast<int>(y * mulRatio + displayOffsetY);
	rect.w = static_cast<int>(w * mulRatio);
	rect.h = static_cast<int>(h * mulRatio);
	SDL_RenderDrawRect(renderer, &rect);
}
#pragma endregion

int main(int argc, char *argv[])
{
	#pragma region INITALISATION
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL_Init HAS FAILED. SDL_ERROR : " << SDL_GetError() << std::endl;
	}

	// Create windows
	SDL_Window* window = SDL_CreateWindow("The Runner 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	// Check windows creation
	if (window == NULL)
	{
		std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event event;
	bool gameRunning = true;
	#pragma endregion

	// Game initialisation
	srand(time(0));
	intersections = generateIntersections();

	// Game loop
	while (gameRunning) {
		// Get our controls and events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameRunning = false;
			}
		}

		#pragma region INPUTS
		if (GetKeyState(VK_SPACE) & 0x8000) {
			if (!spaceDown) {
				spaceDown = true;
			}
		}
		else {
			spaceDown = false;
		}
		#pragma endregion

		#pragma region DRAW
		SDL_RenderClear(renderer);

		#pragma region CALCULATE ASPECT
		int screenWidth, screenHeight;
		SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
		double aspectRatio = (DISPLAY_WIDTH / static_cast<double>(DISPLAY_HEIGHT));
		bool resizeHeight = screenHeight >= screenWidth / aspectRatio;

		double displayWidth = resizeHeight ? screenWidth : screenHeight * aspectRatio;
		double displayHeight = resizeHeight ? screenWidth / aspectRatio : screenHeight;
		displayOffsetX = resizeHeight ? 0 : (screenWidth - displayWidth) / 2;
		displayOffsetY = resizeHeight ? (screenHeight - displayHeight) / 2 : 0;

		mulRatio = displayWidth / static_cast<double>(DISPLAY_WIDTH);
		#pragma endregion

		// intersections
		xCellSize = DISPLAY_WIDTH / GRID_SIZE;
		yCellSize = DISPLAY_HEIGHT / GRID_SIZE;

		for (int y = 0; y <= GRID_SIZE; y++) {
			for (int x = 0; x <= GRID_SIZE; x++) {
				int grayScale = 255 - intersections[y][x] * 255;
				SDL_SetRenderDrawColor(renderer, grayScale, grayScale, grayScale, 255);
				drawRectangle(x * xCellSize - 5, y * yCellSize - 5, 10, 10);
			}
		}

		// lines
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int y = 0; y < GRID_SIZE; y++) {
			for (int x = 0; x < GRID_SIZE; x++) {
				drawLines(x, y);
			}
		}

		// Bounds
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_Rect aspectRect;
		aspectRect.x = displayOffsetX;
		aspectRect.y = displayOffsetY;
		aspectRect.w = displayWidth;
		aspectRect.h = displayHeight;
		SDL_RenderDrawRect(renderer, &aspectRect);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderPresent(renderer);
		#pragma endregion
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}