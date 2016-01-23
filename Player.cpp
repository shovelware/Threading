#include "Player.h"

Player::Player()
{
	//DEBUG_MSG("Constructing Player");
}

Player::~Player()
{
	//DEBUG_MSG("Destructing Player");
}

void Player::Initialize(const float x, const float y)
{
	//Overloaded init for speed and such
	Actor::Initialize(x, y);
	speed_ = 0.75f;

	hpMAX_ = 4;
	hp_ = hpMAX_;
	init_ = true;
}
