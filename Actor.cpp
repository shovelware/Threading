#include "Actor.hpp"

Actor::Actor() : init_(false)
{
	//DEBUG_MSG("Constructing Actor");
}

Actor::~Actor()
{
	//DEBUG_MSG("Destructing Actor");
}

void Actor::Initialize(const float x, const float y)
{
	//_CHANGE_ ENEMY PROPERTIES
	//DEBUG_MSG("Actor Initializing");
	m_y = y;
	m_x = x;
	m_w = 32;
	m_h = 32;

	d_x = 0;
	d_y = 0;

	speed_ = 0.125f;

	hpMAX_ = 1;
	hp_ = hpMAX_;
	init_ = true;
}

void Actor::Render(SDL_Renderer* r)
{
	if (init_)
	{
		//DEBUG_MSG("Actor Drawing");

		SDL_Rect destRect;
		destRect.x = m_x;
		destRect.y = m_y;
		destRect.w = m_w;
		destRect.h = m_h;

		SDL_RenderCopy(r, texture_, NULL, &destRect);
	}
}

void Actor::move(int x, int y)
{
	if (x != 0) { x = (x > 0 ? 1 : -1); }
	if (y != 0) { y = (y > 0 ? 1 : -1); }

	d_x += x;
	d_y += y;
}

void Actor::Update(int dt)
{
	//Jury-rigged normalise
	if (d_x != 0 && d_y != 0)
	{
		d_x /= 1.41;
		d_y /= 1.41;
	}

	//Apply update with speed and deltaTime
	if (d_x != 0) { m_x += (d_x * speed_) * dt; }
	if (d_y != 0) { m_y += (d_y * speed_) * dt; }

	//Reset differentials
	d_x = 0; d_y = 0;
}

void Actor::CleanUp()
{
	DEBUG_MSG("Cleaning Up Actor");

}

void Actor::takeDamage(int damage)
{
	hp_ -= damage;
}

bool Actor::isAlive()
{
	return hp_ > 0;
}
