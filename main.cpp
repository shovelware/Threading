#include <iostream>
#include <string>
#include <chrono>

#include <SDL.h>
#include <SDL_thread.h>

#include "Game.h"
#include "Player.h"

//_CHANGE_ signifies a game-important variable, ctrl+f
struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}
	Point(const Point& p) : x(p.x), y(p.y) {}
};

typedef pair<Enemy*, SDL_Rect> enemyUpdate;
typedef pair<string, Point> hudMessage;

class EnemyBuffer {
public:
	EnemyBuffer() :
		front(0),
		back(0),
		count(0),
		not_empty(SDL_CreateSemaphore(0)),
		not_full(SDL_CreateSemaphore(bufsize))
	{}

	enemyUpdate take()
	{
		while (count == 0)
		{
			SDL_SemWait(not_empty);
		}
		if (SDL_LockMutex(mutter) == 0);
		//CO
		enemyUpdate result = make_pair(buffer[front].first, buffer[front].second);
		front = (front + 1) % bufsize;
		count--;
		//OC
		SDL_UnlockMutex(mutter);
		SDL_SemPost(not_full);
		return result;
	}

	void put(enemyUpdate d)
	{
		while (count == bufsize - 1)
		{
			SDL_SemWait(not_full);
		}
		if (SDL_LockMutex(mutter) == 0)
		//CO
		buffer[back] = d; 
		back = (back + 1) % bufsize;
		count++;
		//OC
		SDL_UnlockMutex(mutter);
		SDL_SemPost(not_empty);
	}

	~EnemyBuffer() { SDL_DestroySemaphore(not_empty); SDL_DestroySemaphore(not_full); SDL_DestroyMutex(mutter); }
	EnemyBuffer(const EnemyBuffer&) {}

private:
	static const int bufsize = 4;
	enemyUpdate buffer[bufsize];
	
	int front;
	int back;
	int count;

	SDL_semaphore* not_empty;
	SDL_semaphore* not_full;
	SDL_mutex* mutter = SDL_CreateMutex();
};

class MessageBuffer {
public:
	MessageBuffer() :
		front(0),
		back(0),
		count(0),
		not_empty(SDL_CreateSemaphore(0)),
		not_full(SDL_CreateSemaphore(bufsize))
	{}

	hudMessage take()
	{
		while (count == 0)
		{
			SDL_SemWait(not_empty);
		}
		if (SDL_LockMutex(mutter) == 0);
		//CO
		hudMessage result = make_pair(buffer[front].first, buffer[front].second);
		front = (front + 1) % bufsize;
		count--;
		//OC
		SDL_UnlockMutex(mutter);
		SDL_SemPost(not_full);
		return result;
	}

	void put(string m, Point p)
	{
		while (count == bufsize - 1)
		{
			SDL_SemWait(not_full);
		}
		if (SDL_LockMutex(mutter) == 0)
			//CO
			buffer[back] = std::make_pair(m, p);
			back = (back + 1) % bufsize;
			count++;
			//OC
		SDL_UnlockMutex(mutter);
		SDL_SemPost(not_empty);
	}

	~MessageBuffer() { SDL_DestroySemaphore(not_empty); SDL_DestroySemaphore(not_full); SDL_DestroyMutex(mutter); }
	MessageBuffer(const MessageBuffer&) {}
	int getCount() { return count; }
private:
	static const int bufsize = 8;
	hudMessage buffer[bufsize];

	int front;
	int back;
	int count;

	SDL_semaphore* not_empty;
	SDL_semaphore* not_full;
	SDL_mutex* mutter = SDL_CreateMutex();
};

using namespace std;

static SDL_mutex* mutti = SDL_CreateMutex();

static SDL_semaphore* enemySem = SDL_CreateSemaphore(1);

EnemyBuffer updatesBuffer = EnemyBuffer();
MessageBuffer hudBuffer = MessageBuffer();

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
		std::list<Enemy>& enemies = g->getEnemies();
			//For each enemy
		for (std::list<Enemy>::iterator iter = enemies.begin(); iter != enemies.end(); iter++)
		{
			//If we need to check edges
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
					updatesBuffer.put(make_pair(&(*iter), newRect));
				}
			}
		}
	}

	return 15;
}

int consEdgeCheckFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	while (g->IsRunning())
	{
		enemyUpdate u = updatesBuffer.take();
		u.first->SetX(u.second.x);
		u.first->SetY(u.second.y);
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

int renderHUDFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	chrono::steady_clock clock;
	chrono::steady_clock::time_point lastTickTime;
	lastTickTime = clock.now();
	const chrono::milliseconds HUDMSG = chrono::milliseconds(5000);
	hudMessage blank = make_pair("", Point(0, 0));
	hudMessage msg;
	msg = blank;
	while (g->IsRunning())
	{

		if (SDL_LockMutex(mutti) == 0)
		{
			if (msg.first != "")
			{	
				//std::cout << "hud" << std::endl;
				g->RenderHUD(msg.first, msg.second.x, msg.second.y);
			}

			if (clock.now() > lastTickTime + HUDMSG)
			{
				lastTickTime += HUDMSG;

				if (hudBuffer.getCount() > 0)
				{
					//std::cout << "duh" << std::endl;
					lastTickTime = clock.now();
					hudMessage msg = hudBuffer.take();

				}

				else msg = blank;
			}

			SDL_UnlockMutex(mutti);
		}

	}

	return 9;
}

int collisionFunc(void* data)
{
	Game* g = static_cast<Game*>(data);

	while (g->IsRunning())
	{
		GameObject*	p = g->getPlayer();
		list<Enemy>& enemies = g->getEnemies();

		//For each enemy
		if (!enemies.empty())
		for (list<Enemy>::iterator iter = enemies.begin(); iter != enemies.end(); iter++)
		{
			//If we need to check edg
			if (iter->isAlive())
			{
				SDL_Rect pr;
				pr.x = p->GetX();
				pr.y = p->GetY();
				pr.w = p->GetW();
				pr.h = p->GetH();

				SDL_Rect er;
				er.x = iter->GetX();
				er.y = iter->GetY();
				er.w = iter->GetW();
				er.h = iter->GetH();

				if (SDL_HasIntersection(&pr, &er))
				{
					std::string msg = "";
					float choix = rand() % 4;

					if (choix < 1)
						msg = "BAP!";
					else if (choix < 2)
						msg = "POW!";
					else if (choix < 3)
						msg = "BLIT!";
					else
						msg = "ZOT!";

					hudBuffer.put(msg, Point(er.x + er.w / 2, er.y + er.h / 2));
				}
			}
		}
	}
	return 11;
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
	SDL_Thread* tHUD = nullptr;
	SDL_Thread* tColl = nullptr;

	if (threading)
	{
		//tCheckEdge = SDL_CreateThread(checkEdgeFunc, "Edge Thread", game);
		tRender = SDL_CreateThread(renderFunc, "Render Thread", game);
		//tProd = SDL_CreateThread(prodEdgeCheckFunc, "Producer Thread", game);
		//tCons = SDL_CreateThread(consEdgeCheckFunc, "Consumer Thread", game);
		tHUD = SDL_CreateThread(renderHUDFunc, "HUD Thread", game);
		tColl = SDL_CreateThread(collisionFunc, "Collision Thread", game);
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

	//DEBUG_MSG("Calling Cleanup");
	game->CleanUp();
	game->UnloadContent();

	SDL_DestroySemaphore(enemySem);
	SDL_DestroyMutex(mutti);
	
	return 0;
}