#include <SDL.h>
#include <Windows.h>
#include <iostream>

const double GRAVITY_FORCE = 0.3;
const double MAX_FPS = 120;

const int DISPLAY_WIDTH = 1600;
const int DISPLAY_HEIGHT = 900;

#pragma region PLAYER CONSTANTS
const double PLAYER_SPEED = 4;
const double DASH_SIZE = 150;
const double JUMP_FORCE = 10;
const double DASH_TIME = 15;
#pragma endregion

double lastFrameStarted = 0;
double deltaTime = 1;

int walls[][4] = {
	{0, 800, 600, 850},
	{800, 800, 1400, 850},
	{1550, 500, 1600, 750},
};

#pragma region PLAYER VARIABLES
double playerX = 200;
double playerY = 775;
double playerVelocityY = 0;
double playerHeight = 50;
double playerWidth = 30;
int playerDirection = 0;
bool playerIsGrounded = false;
bool playerCanDash = false;
double playerWallDistance = PLAYER_SPEED;
double playerDashDistance = DASH_SIZE;
#pragma endregion

int remainingDashTime = 0;

bool spaceDown = false;

int main(int argc, char *argv[])
{
	#pragma region INITALISATION
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL_Init HAS FAILED. SDL_ERROR : " << SDL_GetError() << std::endl;
	}

	// Creer la fenêtre
	SDL_Window* window = SDL_CreateWindow("The Runner 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	// Verifier si la fenêtre à bien été crée
	if (window == NULL)
	{
		std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
	}

	// Creer le renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event event;
	bool gameRunning = true;
	double timePerFrame = 1000 / MAX_FPS;
	int frameCounter = 0;
	#pragma endregion

	// Game loop
	while (gameRunning) {
		// Get our controls and events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameRunning = false;
			}
		}
		lastFrameStarted = SDL_GetTicks();

		if (remainingDashTime > 0) {
			remainingDashTime--;
		}
		else {
			#pragma region INPUTS
			if (GetKeyState(VK_SPACE) & 0x8000) {
				if (!spaceDown) {
					spaceDown = true;
					// start level
					if (playerDirection == 0) {
						playerDirection = 1;
					}

					// wall jump
					else if (playerWallDistance == 0) {
						playerVelocityY = -JUMP_FORCE;
						playerDirection *= -1;
						playerCanDash = true;
					}

					// jump
					else if (playerIsGrounded) {
						playerVelocityY = -JUMP_FORCE;
					}

					// dash
					else if (playerCanDash) {
						playerCanDash = false;
						playerX += (playerDashDistance - PLAYER_SPEED) * playerDirection;
						remainingDashTime = DASH_TIME * (playerDashDistance / DASH_SIZE);
					}
				}
			}
			else {
				spaceDown = false;
			}
			#pragma endregion

			#pragma region PHYSICS
			double fallDistance = playerVelocityY;
			double upDistance = playerVelocityY;
			playerIsGrounded = false;
			playerWallDistance = PLAYER_SPEED;
			playerDashDistance = DASH_SIZE;
			for (int i = 0; i < static_cast<int>(sizeof(walls) / sizeof(int[4])); i++) {
				#pragma region GRAVITY
				// check if the wall is under the player
				if (walls[i][1] >= playerY + playerHeight / 2 && walls[i][0] < playerX + playerWidth / 2 && walls[i][2] > playerX - playerWidth / 2) {
					// calculate distance to the ground
					double groundDistance = walls[i][1] - (playerY + playerHeight / 2);

					// if the ground distance is smaller than the fall distance (will be in the wall next frame)
					if (groundDistance <= fallDistance) {
						fallDistance = groundDistance;
						playerIsGrounded = true;
						playerCanDash = true;
					}
				}
				#pragma endregion

				#pragma region WALLS SIDE DASH
				// check if the wall is aligned veritically with the player
				if (walls[i][1] < playerY + playerHeight / 2 && walls[i][3] > playerY - playerHeight / 2) {
					// calculate both distances
					double wallDistanceLeft = (playerX - playerWidth / 2) - walls[i][2];
					double wallDistanceRight = walls[i][0] - (playerX + playerWidth / 2);

					// change left distance if needed
					if (wallDistanceLeft < playerDashDistance && wallDistanceLeft >= 0 && playerDirection == -1) {
						playerDashDistance = wallDistanceLeft;
					}

					// change right distance if needed
					if (wallDistanceRight < playerDashDistance && wallDistanceRight >= 0 && playerDirection == 1) {
						playerDashDistance = wallDistanceRight;
					}
				}
				#pragma endregion

				#pragma region WALLS SIDE
				// check if the wall is aligned veritically with the player
				if (walls[i][1] < playerY + playerHeight / 2 && walls[i][3] > playerY - playerHeight / 2) {
					// calculate both distances
					double wallDistanceLeft = (playerX - playerWidth / 2) - walls[i][2];
					double wallDistanceRight = walls[i][0] - (playerX + playerWidth / 2);

					// change left distance if needed
					if (wallDistanceLeft < playerWallDistance && wallDistanceLeft >= 0 && playerDirection == -1) {
						playerWallDistance = wallDistanceLeft;
					}

					// change right distance if needed
					if (wallDistanceRight < playerWallDistance && wallDistanceRight >= 0 && playerDirection == 1) {
						playerWallDistance = wallDistanceRight;
					}
				}
				#pragma endregion

				#pragma region HEAD COLLLISON
				// check if the wall is on top of the player
				if (walls[i][3] <= playerY - playerHeight / 2 && walls[i][0] < playerX + playerWidth / 2 && walls[i][2] > playerX - playerWidth / 2) {
					// calculate distance to the ground
					double ceilDistance = walls[i][3] - (playerY - playerHeight / 2);

					// if the ground distance is smaller than the fall distance (will be in the wall next frame)
					if (ceilDistance > upDistance) {
						upDistance = ceilDistance;
					}

					// if the upDistance is smaller than the fallDistance (will be in ceiling next frame)
					if (fallDistance < 0 && fallDistance < upDistance) {
						fallDistance = upDistance;
						playerVelocityY = 0;
					}
				}
				#pragma endregion
			}
			playerX += playerWallDistance * playerDirection;
			playerY += fallDistance;

			// change the player y velocity
			if (playerIsGrounded) {
				playerVelocityY = 0;
			}
			else {
				playerVelocityY += GRAVITY_FORCE;
			}
			#pragma endregion
		}

		#pragma region DRAW
		SDL_RenderClear(renderer);

		#pragma region CALCULATE ASPECT
		int screenWidth, screenHeight;
		SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);
		double aspectRatio = (DISPLAY_WIDTH / static_cast<double>(DISPLAY_HEIGHT));
		bool resizeHeight = screenHeight >= screenWidth / aspectRatio;

		int displayWidth = resizeHeight ? screenWidth : screenHeight * aspectRatio;
		int displayHeight = resizeHeight ? screenWidth / aspectRatio : screenHeight;
		int dispayOffsetX = resizeHeight ? 0 : (screenWidth - displayWidth) / 2;
		int dispayOffsetY = resizeHeight ? (screenHeight - displayHeight) / 2 : 0;

		double mulRatio = displayWidth / static_cast<double>(DISPLAY_WIDTH);
		#pragma endregion

		// player
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_Rect playerRect;
		playerRect.x = (static_cast<int>(playerX) - static_cast<int>(playerWidth / 2)) * mulRatio + dispayOffsetX;
		if (remainingDashTime > 0) {
			playerRect.x -= (DASH_SIZE * (remainingDashTime / DASH_TIME) * playerDirection) * mulRatio;
		}
		playerRect.y = (static_cast<int>(playerY) - static_cast<int>(playerHeight / 2)) * mulRatio + dispayOffsetY;
		playerRect.w = static_cast<int>(playerWidth) * mulRatio;
		playerRect.h = static_cast<int>(playerHeight) * mulRatio;
		SDL_RenderDrawRect(renderer, &playerRect);

		// walls
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int i = 0; i < static_cast<int>(sizeof(walls) / sizeof(int[4])); i++) {
			SDL_Rect rect;
			rect.x = walls[i][0] * mulRatio + dispayOffsetX;
			rect.y = walls[i][1] * mulRatio + dispayOffsetY;
			rect.w = (walls[i][2] - walls[i][0]) * mulRatio;
			rect.h = (walls[i][3] - walls[i][1]) * mulRatio;
			SDL_RenderDrawRect(renderer, &rect);
		}

		// Bounds
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_Rect aspectRect;
		aspectRect.x = dispayOffsetX;
		aspectRect.y = dispayOffsetY;
		aspectRect.w = displayWidth;
		aspectRect.h = displayHeight;
		SDL_RenderDrawRect(renderer, &aspectRect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderPresent(renderer);
		#pragma endregion

		#pragma region DELTA TIME
		deltaTime = SDL_GetTicks() - lastFrameStarted;
		int remainingDelay = static_cast<int>(timePerFrame - deltaTime);
		SDL_Delay(remainingDelay >= 0 ? remainingDelay : 0);
		#pragma endregion

		frameCounter++;
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}