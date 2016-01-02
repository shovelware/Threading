#ifndef GAME_H
#define GAME_H
#include "Debug.h"
#include <SDL.h>

#include <list>

#include "Player.h"
#include "Enemy.hpp"

class Game
{
public:
	Game(bool t);
	~Game();
	bool Initialize(const char*, int, int, int, int, int);
	void LoadContent();
	void UnloadContent();
	bool IsRunning();
	void CleanUp();
	void movePlayer();

	void HandleEvents();
	void Update(int dt);
	void Render();

	GameObject* getPlayer();
	std::list<Enemy>& getEnemies();
private:
	bool m_running;
	SDL_Window* m_p_Window;

	SDL_Renderer* m_p_Renderer;
	SDL_Texture* m_p_Texture;

	SDL_Rect m_Source;
	SDL_Rect m_Destination;

	SDL_Rect m_eSource;
	SDL_Rect m_eDestination;

	SDL_Surface* m_p_Surface;

	SDL_Surface* nmeSurf;
	SDL_Texture* nmeText;

	Player* player_;

	//Action bools
	bool u;
	bool d;
	bool l;
	bool r;
	bool a;

	bool gameOn_;
	bool threading_;
	
	int frames_;
	int time_;

	const int maxEnemies_ = 8; // < _CHANGE_ MAXENEMIES
	std::list<Enemy> enemies_;

	void attemptAddEnemy();
	void addEnemy(float x, float y);
	void reset();
	void end();

	void checkEdge(GameObject* go);
	bool checkCollision(GameObject* goA, GameObject* goB);
	void getHeading(GameObject* target, GameObject* position, float& x, float& y);


	int sw_, sh_;

	int spawnCoolDown_;
	unsigned int spawnTime_;
};
#endif

