
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Shader.h"

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

	void SpriteComponent::Draw(Shader* shader)
	{
		glDrawElements(
			GL_TRIANGLES,		// Type of polygon/primitive to draw
			6,					// Number of indices in index buffer
			GL_UNSIGNED_INT,    // Type of each index
			nullptr				// Usually nullptr
		);
	}

	void SpriteComponent::SetTexture(SDL_Texture* texture)
	{
		m_Texture = texture;
		SDL_QueryTexture(texture, nullptr, nullptr, &m_TexWidth, &m_TexHeight);
	}
}