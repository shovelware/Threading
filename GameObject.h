#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <iostream>
#include "Debug.h"
#include "SDL.h"
#include "Vector2D.h"

using namespace std;

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Initialize(const float x, const float y) = 0;
	void setTexture(SDL_Texture* tex);

	virtual void Render(SDL_Renderer* r) = 0;
	virtual void Update(int dt) = 0;
	virtual void CleanUp() = 0;

	float GetX() const;
	float GetY() const;
	float GetW() const;
	float GetH() const;

	void SetX(float);
	void SetY(float);
	void SetW(float);
	void SetH(float);

protected:
	float m_x;
	float m_y;
	float m_w;
	float m_h;

	SDL_Texture* texture_;
	SDL_Rect* srcRect_;

};
#endif
