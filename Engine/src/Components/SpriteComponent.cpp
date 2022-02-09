
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>

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
		if (m_Texture)
		{
			// Scale the quad by the width/height of texture
			Matrix4 scaleMat = Matrix4::CreateScale(
				static_cast<float>(m_TexWidth),
				static_cast<float>(m_TexHeight),
				1.f);

			Matrix4 world = scaleMat * m_Owner->GetWorldTransform();

			// Since all sprites use the same shader/vertices,
			// the game first sets them active before any sprite draws

			shader->SetMatrixUniform("uWorldTransform", world);
			m_Texture->SetActive();

			glDrawElements(
				GL_TRIANGLES,		// Type of polygon/primitive to draw
				6,					// Number of indices in index buffer
				GL_UNSIGNED_INT,    // Type of each index
				nullptr				// Usually nullptr
			);
		};
	}

	void SpriteComponent::SetTexture(Texture* texture)
	{
		m_Texture = texture;
		m_TexWidth = texture->GetWidth();
		m_TexHeight = texture->GetHeight();
	}
}