#include <SDL.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <random>
#include "SDLRender.cpp"

using namespace std;

#pragma region CONSTANTS
const int GRID_SIZE = 20;
const double WALL_VALUE_TOP = 0.5;
const double WALL_VALUE_BOTTOM = 0.5;
#pragma endregion

#pragma region VARIABLES
bool spaceDown = false;

vector<vector<double>> intersections;
#pragma endregion

#pragma region FUNCTIONS
double calculateIntersection(int x, int y) {
	return static_cast<double>(rand() % 1001) / 1000;
}

vector<vector<double>> generateIntersections() {
	vector<vector<double>> grid;
	for (int y = 0; y <= GRID_SIZE; y++) {
		vector<double> row;
		for (int x = 0; x <= GRID_SIZE; x++) {
			row.push_back(calculateIntersection(x, y));
		}
		grid.push_back(row);
	}
	return grid;
}
#pragma endregion

int main(int argc, char *argv[])
{
	#pragma region INITALISATION
	renderInit();

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

		renderMarchingSquares(intersections, GRID_SIZE, WALL_VALUE_TOP, WALL_VALUE_BOTTOM);
	}

	renderDestroy();

	return 0;
}