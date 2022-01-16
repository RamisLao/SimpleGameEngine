#pragma once
#include <vector>
#include "SpriteComponent.h"

namespace Engine
{
	class AnimSpriteComponent : public SpriteComponent
	{
	public:
		AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
		// Update animation every frame (overriden from component)
		void Update(float deltaTime) override;
		//Set the textures used for animation
		void SetAnimTextures(const std::vector<SDL_Texture*>& textures);
		float GetAnimFPS() const { return m_AnimFPS; }
		void SetAnimFPS(float fps) { m_AnimFPS = fps; }
	private:
		std::vector<SDL_Texture*> m_AnimTextures; 
		float m_CurrentFrame;
		float m_AnimFPS;
	};
}