#ifndef ACTOR_H
#define ACTOR_H

#include "GameObject.h"

class Actor : public GameObject
{
public:
	Actor();
	~Actor();
	virtual void Initialize(const float x, const float y);
	void Render(SDL_Renderer* r);
	void Update(int dt);
	void CleanUp();

	void takeDamage(int damage);
	bool isAlive();

	void move(int x, int y);
protected:
	bool init_;
	float speed_;
	int hp_;
	int hpMAX_;

	float d_x;
	float d_y;
};

#endif