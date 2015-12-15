#include <iostream>
#include <thread>
#include <string>
#include <chrono>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>

#include "Game.h"
#include "Player.h"

using namespace std;

static SDL_mutex* mutti = SDL_CreateMutex();

int checkEdgeFunc(void* data)
{
	GameObject* go = static_cast<GameObject*>(data);

	while (true)
	{
		float x, y, w, h;
		x = go->GetX();
		y = go->GetY();
		w = go->GetW();
		h = go->GetH();

		SDL_LockMutex(mutti);
		
		if (x > 800)
		{
			go->SetX(0 - w);
		}

		else if (x + w < 0)
		{
			go->SetX(800);
		}

		//

		if (y > 800)
		{
			go->SetY(0 - h);
		}
		if (y + h < 0)
		{
			go->SetY(800);
		}

		SDL_UnlockMutex(mutti);
	}

	return 42;
}

int renderFunc(void* data)
{
	Game* g = static_cast<Game*>(data);
	while (true)
	{
		SDL_LockMutex(mutti);
		g->Render();
		SDL_UnlockMutex(mutti);
	}

	return 24;
}

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

	bool edgeCheckThread = false;
	bool renderThread = false;


	DEBUG_MSG("Game Loop Starting......");
	while (game->IsRunning())
	{
		while (clock.now() > lastTickTime + TIME_PER_TICK)
		{
			lastTickTime += TIME_PER_TICK;

			game->HandleEvents();
			game->Update(TIME_PER_TICK.count());

			if (!edgeCheckThread)
			{
				SDL_Thread* tEdgeCheck;
				GameObject* player = game->getPlayer();

				tEdgeCheck = SDL_CreateThread(checkEdgeFunc, "Edge Thread", player);
				edgeCheckThread = true;
			}

			//if (!renderThread)
			//{
			//	SDL_Thread* tRender;
			//
			//	tRender = SDL_CreateThread(renderFunc, "Render Thread", game);
			//	renderThread = true;
			//}

			game->Render();
		}
	}

	DEBUG_MSG("Calling Cleanup");
	game->CleanUp();
	game->UnloadContent();
	
	return 0;
}
