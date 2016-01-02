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

static SDL_semaphore* enemySem = SDL_CreateSemaphore(1);

std::list<pair<Enemy*, SDL_Rect>> updatesBuffer;

int checkEdgeFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	while (g->IsRunning())
	{
		GameObject* go = g->getPlayer();
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

int prodEdgeCheckFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	while (g->IsRunning())
	{
		//Lock enemies
		SDL_SemWait(enemySem);
		std::list<Enemy>& enemies = g->getEnemies();
		////CO
		for (std::list<Enemy>::iterator iter = enemies.begin(); iter != enemies.end(); iter++)
		{
			if (iter->isAlive())
			{
				bool addToBuffer = false;
				SDL_Rect newRect;

				newRect.x = iter->GetX();
				newRect.y = iter->GetY();
				newRect.w = iter->GetW();
				newRect.h = iter->GetH();

				//Check edges of enemy
				if (newRect.x > 800)
				{
					addToBuffer = true;
					newRect.x = (0 - newRect.w);
				}

				else if (newRect.x + newRect.w < 0)
				{
					addToBuffer = true;
					newRect.x = 800;
				}

				//

				if (newRect.y > 800)
				{
					addToBuffer = true;
					newRect.y = (0 - newRect.h);
				}
				if (newRect.y + newRect.h < 0)
				{
					addToBuffer = true;
					newRect.y = 800;
				}

				//Push changes to buffer if we need to
				if (addToBuffer)
				{
					updatesBuffer.push_back(make_pair(&(*iter), newRect));
				}
			}
		}
		////OC

		//Unlock enemies
		SDL_SemPost(enemySem);
	}

	return 15;
}

int consEdgeCheckFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	while (g->IsRunning())
	{
		//Lock enemies
		SDL_SemWait(enemySem);
		////CO
		//for (std::pair<Enemy*, SDL_Rect>& u : updatesBuffer)

		for (std::list<pair<Enemy*, SDL_Rect>>::iterator iter = updatesBuffer.begin(); iter != updatesBuffer.end(); iter++)
		{
			iter->first->SetX(iter->second.x);
			iter->first->SetY(iter->second.y);
		}

		//Clear the updates when we're done
		updatesBuffer.clear();

		////OC

		//Unlock enemies
		SDL_SemPost(enemySem);
	}

	return 19;
}

int renderFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	chrono::steady_clock clock;
	chrono::steady_clock::time_point lastTickTime;
	lastTickTime = clock.now();
	const chrono::milliseconds SECOND = chrono::milliseconds(1000);

	int frames = 0;

	while (g->IsRunning())
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
	SDL_Thread* tProd = nullptr;
	SDL_Thread* tCons = nullptr;

	if (threading)
	{
		tCheckEdge = SDL_CreateThread(checkEdgeFunc, "Edge Thread", game);
		tRender = SDL_CreateThread(renderFunc, "Render Thread", game);
		tProd = SDL_CreateThread(prodEdgeCheckFunc, "Producer Thread", game);
		tCons = SDL_CreateThread(consEdgeCheckFunc, "Consumer Thread", game);
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

	int op;
	SDL_WaitThread(tCheckEdge, &op);
	SDL_WaitThread(tRender, &op);
	SDL_WaitThread(tProd, &op);
	SDL_WaitThread(tCons, &op);
	

	//DEBUG_MSG("Calling Cleanup");
	game->CleanUp();
	game->UnloadContent();
	
	return 0;
}
