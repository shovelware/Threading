#include "Enemy.hpp"

Enemy::Enemy()
{
	DEBUG_MSG("Constructing Enemy");
}

Enemy::~Enemy()
{
	DEBUG_MSG("Destructing Enemy");
}

void Enemy::Initialize(const float x, const float y)
{
	DEBUG_MSG("Enemy Initializing");
	m_y = y;
	m_x = x;
	m_w = 32;
	m_h = 32;

	d_x = 0;
	d_y = 0;

	speed_ = 0.125f;

	hpMAX_ = 1;
	hp_ = hpMAX_;
}

void Enemy::Render(SDL_Renderer* r)
{
	//DEBUG_MSG("Enemy Drawing");

	SDL_Rect destRect;
	destRect.x = m_x;
	destRect.y = m_y;
	destRect.w = m_w;
	destRect.h = m_h;

	SDL_RenderCopy(r, texture_, NULL, &destRect);
}

void Enemy::move(int x, int y)
{
	if (x != 0) { x = (x > 0 ? 1 : -1); }
	if (y != 0) { y = (y > 0 ? 1 : -1); }

	d_x += x;
	d_y += y;
}

void Enemy::Update(int dt)
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

void Enemy::CleanUp()
{
	DEBUG_MSG("Cleaning Up Enemy");

}

void Enemy::takeDamage(int damage)
{
	hp_ -= damage;
}

bool Enemy::isAlive()
{
	return hp_ > 0;
}
