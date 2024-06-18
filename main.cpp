#include <SDL.h>
#include <Windows.h>
#include <iostream>

#pragma region CONSTANTS
const int DISPLAY_WIDTH = 1000;
const int DISPLAY_HEIGHT = 1000;
#pragma endregion

#pragma region VARIABLES
bool spaceDown = false;

SDL_Renderer* renderer;
double displayOffsetX, displayOffsetY, mulRatio;
#pragma endregion

void drawRectangle(double x, double y, double w, double h) {
	SDL_Rect rect;
	rect.x = static_cast<int>(x * mulRatio + displayOffsetX);
	rect.y = static_cast<int>(y * mulRatio + displayOffsetY);
	rect.w = static_cast<int>(w * mulRatio);
	rect.h = static_cast<int>(h * mulRatio);
	SDL_RenderDrawRect(renderer, &rect);
}

int main(int argc, char *argv[])
{
	#pragma region INITALISATION
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL_Init HAS FAILED. SDL_ERROR : " << SDL_GetError() << std::endl;
	}

	// Create windows
	SDL_Window* window = SDL_CreateWindow("The Runner 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

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

		// test cube
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		drawRectangle(50, 50, 900, 900);

		// Bounds
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_Rect aspectRect;
		aspectRect.x = displayOffsetX;
		aspectRect.y = displayOffsetY;
		aspectRect.w = displayWidth;
		aspectRect.h = displayHeight;
		SDL_RenderDrawRect(renderer, &aspectRect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderPresent(renderer);
		#pragma endregion
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}