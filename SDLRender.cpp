#include <SDL.h>
#include <vector>

using namespace std;

#pragma region CONSTANTS
const int DISPLAY_WIDTH = 1000;
const int DISPLAY_HEIGHT = 1000;
#pragma endregion

#pragma region VARIABLES
SDL_Window* window;
SDL_Renderer* renderer;
double displayOffsetX, displayOffsetY, mulRatio, xCellSize, yCellSize;
#pragma endregion

void renderInit() {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		cout << "SDL_Init HAS FAILED. SDL_ERROR : " << SDL_GetError() << endl;
	}

	// Create windows
	window = SDL_CreateWindow("The Runner 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	// Check windows creation
	if (window == NULL)
	{
		cout << "Window failed to init. Error: " << SDL_GetError() << endl;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

#pragma region SHAPES
void drawRectangle(double x, double y, double w, double h) {
	SDL_Rect rect;
	rect.x = static_cast<int>(x * mulRatio + displayOffsetX);
	rect.y = static_cast<int>(y * mulRatio + displayOffsetY);
	rect.w = static_cast<int>(w * mulRatio);
	rect.h = static_cast<int>(h * mulRatio);
	SDL_RenderDrawRect(renderer, &rect);
}
#pragma endregion

void renderMarchingSquares(vector<vector<double>> intersections, int gridSize, double wallValueTop, double wallValueBottom) {
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
	xCellSize = DISPLAY_WIDTH / gridSize;
	yCellSize = DISPLAY_HEIGHT / gridSize;

	for (int y = 0; y <= gridSize; y++) {
		for (int x = 0; x <= gridSize; x++) {
			int grayScale = 255 - intersections[y][x] * 255;
			SDL_SetRenderDrawColor(renderer, grayScale, grayScale, grayScale, 255);
			drawRectangle(x * xCellSize - 5, y * yCellSize - 5, 10, 10);
		}
	}

	// lines
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int y = 0; y < gridSize; y++) {
		for (int x = 0; x < gridSize; x++) {
			// calculate the wall values
			const double wallValue1 = (wallValueBottom - wallValueTop) * (y / gridSize) + wallValueTop;
			const double wallValue2 = (wallValueBottom - wallValueTop) * ((y + 1) / gridSize) + wallValueTop;

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
}

void renderDestroy() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}