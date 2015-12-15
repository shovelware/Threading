#include <iostream>
#include <thread>
#include <string>
#include <chrono>

#include <SDL.h>
#include <SDL_thread.h>

#include "Game.h"
#include "Player.h"

//_CHANGE_ signifies a game-important variable, ctrl+f

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

		 if (SDL_LockMutex(mutti) == 0);
		 {
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
	}

	return 42;
}

int renderFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	chrono::steady_clock clock;
	chrono::steady_clock::time_point lastTickTime;
	lastTickTime = clock.now();
	const chrono::milliseconds SECOND = chrono::milliseconds(1000);

	int frames = 0;

	while (true)
	{
		
		if (SDL_LockMutex(mutti) == 0)
		{
			g->Render();
			frames++;
			SDL_UnlockMutex(mutti);
		}

		if (clock.now() > lastTickTime + SECOND)
		{
			lastTickTime += SECOND;
			cout << "\tFPS: " << frames << endl;
			frames = 0;
		}
	}

	return 24;
}

int main(int argc, char** argv)
{
	//ENABLE OR DISABLE THREADING
	bool threading = true;
	//_CHANGE_ HERE ^^^
	
	//DEBUG_MSG("Game Object Created");
	Game* game = new Game(threading);

	//Adjust screen positions as needed
	//DEBUG_MSG("Game Initialising");
	game->Initialize("ThreadGame",600,200,800,800, SDL_WINDOW_INPUT_FOCUS);
	
	//DEBUG_MSG("Loading Content");
	game->LoadContent();

	const chrono::milliseconds TIME_PER_TICK = chrono::milliseconds(16);


	chrono::steady_clock clock;
	chrono::steady_clock::time_point lastTickTime;
	lastTickTime = clock.now();

	SDL_Thread* tCheckEdge = nullptr;
	SDL_Thread* tRender = nullptr;

	if (threading)
	{
		tCheckEdge = SDL_CreateThread(checkEdgeFunc, "Edge Thread", game->getPlayer());
		tRender = SDL_CreateThread(renderFunc, "Render Thread", game);
	}

	//DEBUG_MSG("Game Loop Starting......");
	while (game->IsRunning())
	{
		while (clock.now() > lastTickTime + TIME_PER_TICK)
		{
			lastTickTime += TIME_PER_TICK;

			game->HandleEvents();
			game->Update(TIME_PER_TICK.count());

			if (!threading)
			{
				game->Render();
			}
		}
	}

	SDL_DetachThread(tCheckEdge);
	SDL_DetachThread(tRender);

	//DEBUG_MSG("Calling Cleanup");
	game->CleanUp();
	game->UnloadContent();
	
	return 0;
}
