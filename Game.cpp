#include "Game.h"
#include <iostream>
#include <time.h>

using namespace std;

inline float randFloat(float MAX) { return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / MAX)); };
inline float randFloat(float MIN, float MAX) { return MIN + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (MAX - MIN))); };

Game::Game() : m_running(false), gameOn_(false)
{
	u = false;
	d = false;
	l = false;
	r = false;
	a = false;
}

Game::~Game()
{
}

bool Game::Initialize(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		DEBUG_MSG("SDL Init success");
		m_p_Window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

		sw_ = width;
		sh_ = height;

		if(m_p_Window != 0)
		{
			DEBUG_MSG("Window creation success");
			m_p_Renderer = SDL_CreateRenderer(m_p_Window, -1, 0);
			if(m_p_Renderer != 0)
			{
				DEBUG_MSG("Renderer creation success");
				SDL_SetRenderDrawColor(m_p_Renderer, 65, 50, 100, 255);
			}
			else
			{
				DEBUG_MSG("Renderer init fail");
				return false;
			}
		}
		else
		{
			DEBUG_MSG("Window init fail");
			return false;
		}
	}
	else
	{
		DEBUG_MSG("SDL init fail");
		return false;
	}
	m_running = true;
	player_ = new Player();
	spawnTime_ = 2000U;

	srand(time(NULL));

	reset();
	return true;
}

void Game::LoadContent()
{
	DEBUG_MSG("Loading Content");
	m_p_Surface = SDL_LoadBMP("assets/player.bmp");
	m_p_Texture = SDL_CreateTextureFromSurface(m_p_Renderer, m_p_Surface);
	SDL_FreeSurface(m_p_Surface);

	if(SDL_QueryTexture(m_p_Texture, NULL, NULL, &m_Source.w, &m_Source.h)==0)
	{
		m_Destination.x = m_Source.x = 0;
		m_Destination.y = m_Source.y = 0;
		m_Destination.w = m_Source.w;
		m_Destination.h = m_Source.h;

		//DEBUG_MSG("Destination X:" + m_Destination.x);
		//DEBUG_MSG("Destination Y:" + m_Destination.y);
		//DEBUG_MSG("Destination W:" + m_Destination.w);
		//DEBUG_MSG("Destination H:" + m_Destination.h);
	}

	else
	{
		DEBUG_MSG("Texture Query Failed");
		m_running = false;
	}

	player_->setTexture(m_p_Texture);

	////
	nmeSurf = SDL_LoadBMP("assets/enemy.bmp");
	nmeText = SDL_CreateTextureFromSurface(m_p_Renderer, nmeSurf);
	SDL_FreeSurface(nmeSurf);

	if (SDL_QueryTexture(nmeText, NULL, NULL, &m_Source.w, &m_Source.h) == 0)
	{
		m_Destination.x = m_Source.x = 0;
		m_Destination.y = m_Source.y = 0;
		m_Destination.w = m_Source.w;
		m_Destination.h = m_Source.h;

		//DEBUG_MSG("Destination X:" + m_Destination.x);
		//DEBUG_MSG("Destination Y:" + m_Destination.y);
		//DEBUG_MSG("Destination W:" + m_Destination.w);
		//DEBUG_MSG("Destination H:" + m_Destination.h);
	}

	else
	{
		DEBUG_MSG("Texture Query Failed");
		m_running = false;
	}
}

void Game::attemptAddEnemy()
{
	//Coin flip to spawn another
	if (randFloat(2) > 1.0f)
	{
		int loops = 0;

		float px = player_->GetX();
		float py = player_->GetY();
		float esx = px;
		float esy = py;

		float xdist = sw_ / 2;
		float ydist = sh_ / 2;

		while (abs(esx - px) < xdist && loops < 5)
		{
			esx = randFloat(0, sw_);
			loops++;
		}

		while (abs(esy = py) < ydist && loops < 5)
		{
			esy = randFloat(0, sh_);
			loops++;
		}

		if (loops < 10)
		{
			addEnemy(esx, esy);
		}
	}

	cout << "Spawn Roll Failed" << endl;

	spawnCoolDown_ = spawnTime_;
}

void Game::addEnemy(float x, float y)
{
	if (enemies_.size() < maxEnemies_)
	{
		enemies_.emplace_back();
		spawnCoolDown_ = spawnTime_;
	}
	
	(--enemies_.end())->Initialize(x, y);
	(--enemies_.end())->setTexture(nmeText);
}

void Game::reset()
{
	enemies_.clear();

	player_->Initialize(sw_ / 2 - 32, sh_ / 2 - 32);

	gameOn_ = true;
	spawnCoolDown_ = spawnTime_;
}

void Game::end()
{
	gameOn_ = false;
}

bool Game::IsRunning()
{
	return m_running;
}

void Game::UnloadContent()
{
	DEBUG_MSG("Unloading Content");
	//delete(m_p_Texture);
	//m_p_Texture = NULL;
}

void Game::CleanUp()
{
	DEBUG_MSG("Cleaning Up");
	delete player_;
	SDL_DestroyWindow(m_p_Window);
	SDL_DestroyRenderer(m_p_Renderer);
	SDL_Quit();
}

void Game::checkEdge(GameObject* go)
{
	float x, y, w, h;
	x = go->GetX();
	y = go->GetY();
	w = go->GetW();
	h = go->GetH();

	if (x > sw_)
	{
		go->SetX(0 - w);
	}

	else if (x + w < 0)
	{
		go->SetX(sw_);
	}

	//

	if (y > sh_)
	{
		go->SetY(0 - h);
	}
	if (y + h < 0)
	{
		go->SetY(sh_);
	}
}

bool Game::checkCollision(GameObject * goA, GameObject * goB)
{
	SDL_Rect a;
	a.x = goA->GetX();
	a.y = goA->GetY();
	a.w = goA->GetW();
	a.h = goA->GetH();

	SDL_Rect b;
	b.x = goB->GetX();
	b.y = goB->GetY();
	b.w = goB->GetW();
	b.h = goB->GetH();

	return SDL_HasIntersection(&a, &b);
}

void Game::getHeading(GameObject* target, GameObject* position, float& x, float& y)
{
	x = target->GetX() - position->GetX();
	y = target->GetY() - position->GetY();
}

void Game::movePlayer() {

	int x = 0;
	int y = 0;

	if (l ^ r)
	{
		if (l)
			x = -1;

		else if (r)
			x = 1;
	}

	if (u ^ d)
	{
		if (u)
			y = -1;

		else if (d)
			y = 1;
	}

	player_->move(x, y);
}

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			m_running = false;
		}

		else if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_running = false;
				break;
			case SDLK_RETURN:
				break;

			case SDLK_F5:
				reset();
				break;

			case SDLK_UP:
				u = true;
				break;
			case SDLK_DOWN:
				d = true;
				break;
			case SDLK_LEFT:
				l = true;
				break;
			case SDLK_RIGHT:
				r = true;
				break;
			case SDLK_SPACE:
				a = true;
				break;

			case SDLK_w:
				u = true;
				break;
			case SDLK_s:
				d = true;
				break;
			case SDLK_a:
				l = true;
				break;
			case SDLK_d:
				r = true;
				break;
			default:
				break;
			}
		}

		else if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_running = false;
				break;

			case SDLK_UP:
				u = false;
				break;
			case SDLK_DOWN:
				d = false;
				break;
			case SDLK_LEFT:
				l = false;
				break;
			case SDLK_RIGHT:
				r = false;
				break;

			case SDLK_w:
				u = false;
				break;
			case SDLK_s:
				d = false;
				break;
			case SDLK_a:
				l = false;
				break;
			case SDLK_d:
				r = false;
				break;

			case SDLK_SPACE:
				a = false;
				break;
			default:
				break;

			}
		}
	}

}

void Game::Update(int dt)
{
	//DEBUG_MSG("Updating....");
	
	if (gameOn_)
	{
		if (spawnCoolDown_ > 0)
		{
			spawnCoolDown_ -= dt;
		}

		//Player update
		movePlayer();
		player_->Update(dt);

		//Enemy Spawn
		//If we're not full, add one
		if (spawnCoolDown_ <= 0 && !enemies_.size() < maxEnemies_)
		{
			attemptAddEnemy();
		}

		//Enemies update
		if (!enemies_.empty())
		{
			float emx, emy;

			for (std::list<Enemy>::iterator e = enemies_.begin();
				e != enemies_.end(); /*Don't increment here*/)
			{
				//If we're alive
				if (e->isAlive())
				{
					emx = 0; emy = 0;

					//Get direction to player
					getHeading(player_, &*e, emx, emy);

					//Enemy update
					e->move(emx, emy);
					e->Update(dt);
					checkEdge(&*e);

					//Collision check
					if (checkCollision(player_, &*e))
					{
						player_->takeDamage(1);
						e->takeDamage(1);
					}

					++e;
				}

				//If we're not alive, increment by deleting
				else if (e->isAlive() == false)
				{
					enemies_.erase(e++);
				}
			}
		}

		//every enemy update towards player (T)

		//Every edgecheck for enemy
	}

	else if (a)
	{
		reset();
	}
}

void Game::Render()
{
	SDL_RenderClear(m_p_Renderer);
	//DEBUG_MSG("Width Source" + m_Destination.w);
	//DEBUG_MSG("Width Destination" + m_Destination.w);

	if (m_p_Renderer != nullptr && m_p_Texture != nullptr)
	{
		//Render player
		player_->Render(m_p_Renderer);

		//Render enemies
		if (!enemies_.empty())
		{
			for (std::list<Enemy>::iterator e = enemies_.begin();
			e != enemies_.end(); ++e)
			{
				e->Render(m_p_Renderer);
			}
		}

		//SDL_RenderCopy(m_p_Renderer, m_p_Texture, NULL, NULL);
	}

	SDL_RenderPresent(m_p_Renderer);

}

GameObject* Game::getPlayer()
{
	return static_cast<GameObject*>(player_);
}
