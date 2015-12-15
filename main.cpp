#include <iostream>
#include <thread>
#include <string>
#include <chrono>

#include <SDL.h>
#include <SDL_image.h>

#include "Game.h"
#include "Player.h"

using namespace std;

int main(int argc, char** argv){
	DEBUG_MSG("Game Object Created");
	
	Game* game = new Game();
	
	//Adjust screen positions as needed
	DEBUG_MSG("Game Initialising");
	game->Initialize("ThreadGame",600,200,800,800, SDL_WINDOW_INPUT_FOCUS);
	
	DEBUG_MSG("Loading Content");
	game->LoadContent();

	const chrono::milliseconds TIME_PER_TICK = chrono::milliseconds(16);

	chrono::steady_clock clock;
	chrono::steady_clock::time_point lastTickTime;
	lastTickTime = clock.now();

	DEBUG_MSG("Game Loop Starting......");
	while (game->IsRunning())
	{
		while (clock.now() > lastTickTime + TIME_PER_TICK)
		{
			lastTickTime += TIME_PER_TICK;

			game->HandleEvents();
			game->Update(TIME_PER_TICK.count());
			game->Render();
		}
	}

	DEBUG_MSG("Calling Cleanup");
	game->CleanUp();
	game->UnloadContent();
	
	return 0;
}


