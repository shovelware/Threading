#ifndef PLAYER_H
#define PLAYER_H

#include "Actor.hpp"

class Player : public Actor
{
public:
	Player();
	~Player();
	void Initialize(const float x, const float y); 
};
#endif