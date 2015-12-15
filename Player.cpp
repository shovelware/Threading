#include "Player.h"

Player::Player() : init_(false)
{
	//DEBUG_MSG("Constructing Player");
}

Player::~Player()
{
	//DEBUG_MSG("Destructing Player");
}

void Player::Initialize(const float x, const float y)
{
	//_CHANGE_ PLAYER PROPERTIES
	//DEBUG_MSG("Player Initializing");
	m_y = y;
	m_x = x;
	m_w = 32;
	m_h = 32;

	d_x = 0;
	d_y = 0;

	speed_ = 0.75f;

	hpMAX_ = 4;
	hp_ = hpMAX_;
	init_ = true;
}

void Player::Render(SDL_Renderer* r)
{
	if (init_)
	{
		//DEBUG_MSG("Player Drawing");

		SDL_Rect destRect;
		destRect.x = m_x;
		destRect.y = m_y;
		destRect.w = m_w;
		destRect.h = m_h;

		SDL_RenderCopy(r, texture_, NULL, &destRect);
	}
}

void Player::move(int x, int y)
{
	if (x != 0) { x = (x > 0 ? 1 : -1);	}
	if (y != 0) { y = (y > 0 ? 1 : -1); }

	d_x += x;
	d_y += y;
}

void Player::Update(int dt)
{
	//DEBUG_MSG("Player Updating");

	//Jury-rigged normalise
	if (d_x != 0 && d_y != 0)
	{
		d_x /= 1.41;
		d_y /= 1.41;
	}

	//Apply update with speed and deltaTime
	if (d_x != 0) { m_x += (d_x * speed_) * dt; }
	if (d_y != 0) { m_y += (d_y * speed_) * dt; }

	
	
	////Other update stuff////



	//Reset differentials
	d_x = 0; d_y = 0;
}

void Player::CleanUp()
{
	DEBUG_MSG("Cleaning Up Player");
	
}

void Player::takeDamage(int damage)
{
	hp_ -= damage;
}

bool Player::isAlive()
{
	return hp_ > 0;
}
