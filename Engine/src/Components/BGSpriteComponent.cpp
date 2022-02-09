
#include "BGSpriteComponent.h"
#include "Actor.h"
#include "Texture.h"

namespace Engine
{
	BGSpriteComponent::BGSpriteComponent(Actor* owner, int drawOrder) :
		SpriteComponent(owner, drawOrder),
		m_ScrollSpeed(0.0f)
	{
	}

	void BGSpriteComponent::Update(float deltaTime)
	{
		SpriteComponent::Update(deltaTime);
		for (auto& bg : m_BGTextures)
		{
			bg.m_Offset.x += m_ScrollSpeed * deltaTime;
			// If this is completely off the screen, reset offset to the
			// right of the last bg texture
			if (bg.m_Offset.x < -m_ScreenSize.x)
			{
				bg.m_Offset.x = (m_BGTextures.size() - 1) * m_ScreenSize.x - 1;
			}
		}
	}

	void BGSpriteComponent::Draw(Shader* shader)
	{
		
	}

	void BGSpriteComponent::SetBGTextures(const std::vector<Texture*>& textures)
	{
		int count = 0;
		for (auto tex : textures)
		{
			BGTexture temp;
			temp.m_Texture = tex;
			temp.m_Offset.x = count * m_ScreenSize.x;
			temp.m_Offset.y = 0;
			m_BGTextures.emplace_back(temp);
			count++;
		}
	}
}