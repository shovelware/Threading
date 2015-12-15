#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();
	~Player();
	void Initialize(const float x, const float y); 
	void Render(SDL_Renderer* r);
	void Update(int dt);
	void CleanUp();

	void takeDamage(int damage);
	bool isAlive();

	void move(int x, int y);
private:
	float speed_;
	int hp_;
	int hpMAX_;

	float d_x;
	float d_y;
};
#endif