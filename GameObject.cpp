#include "GameObject.h"

GameObject::GameObject()
{
	DEBUG_MSG("Character Constructor");
}

GameObject::~GameObject()
{
	DEBUG_MSG("Character Destructor");
}

void GameObject::setTexture(SDL_Texture* tex) { texture_ = tex; }

float GameObject::GetX() const { return m_x; }
float GameObject::GetY() const { return m_y; }
float GameObject::GetW() const { return m_w; }
float GameObject::GetH() const { return m_h; }

void GameObject::SetX(float x) { m_x = x; }
void GameObject::SetY(float y) { m_y = y; }
void GameObject::SetW(float w) { m_w = w; }
void GameObject::SetH(float h) { m_h = h; }

