
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

namespace Engine
{
	SpriteComponent::SpriteComponent(Actor* owner, int drawOrder) :
		Component(owner),
		m_Texture(nullptr),
		m_DrawOrder(drawOrder),
		m_TexWidth(0),
		m_TexHeight(0)
	{
		m_Owner->GetGame()->AddSprite(this);
	}

	SpriteComponent::~SpriteComponent()
	{
		m_Owner->GetGame()->RemoveSprite(this);
	}

	void SpriteComponent::Draw(SDL_Renderer* renderer)
	{
		if (m_Texture)
		{
			SDL_Rect r;
			// Scale the width/height by owner's scale
			r.w = static_cast<int>(m_TexWidth * m_Owner->GetScale());
			r.h = static_cast<int>(m_TexHeight * m_Owner->GetScale());
			// Center the rectangle around the position of the owner
			r.x = static_cast<int>(m_Owner->GetPosition().x - r.w / 2);
			r.y = static_cast<int>(m_Owner->GetPosition().y - r.h / 2);

			// Draw (have to convert angle from radians to degrees, and clockwise)
			SDL_RenderCopyEx(renderer,	// Render target to draw to
				m_Texture,				// Texture to draw
				nullptr,				// Part of texture to draw (null if whole)
				&r,						// Rectangle to draw onto the target
				-CustomMath::ToDegrees(m_Owner->GetRotation()), // Rotation angle (in degrees, clockwise, opposite to the unit circle)
				nullptr,				// Point to rotate about (nullptr for center)
				SDL_FLIP_NONE);			// How to flip texture (usually SDL_FLIP_NONE
		}
	}

	void SpriteComponent::SetTexture(SDL_Texture* texture)
	{
		m_Texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &m_TexWidth, &m_TexHeight);
	}
}