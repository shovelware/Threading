#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"

class Enemy : public GameObject 
{
public:
	Enemy();
	~Enemy();
	void Initialize(const float x, const float y);
	void Render(SDL_Renderer* r);
	void Update(int dt);
	void CleanUp();

	void takeDamage(int damage);
	bool isAlive();

	void move(int x, int y);
private:
	bool init_;
	float speed_;
	int hp_;
	int hpMAX_;

	float d_x;
	float d_y;
};
#endif