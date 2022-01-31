#pragma once
#include "Component.h"
#include "SDL.h"
#include "CustomMath.h"

namespace Engine
{
	class SpriteComponent : public Component
	{
	public:
		// Lower draw order corresponds with further back
		SpriteComponent(class Actor* owner, int drawOrder = 100);
		~SpriteComponent();

		virtual void Draw(class Shader* shader);
		virtual void SetTexture(SDL_Texture* texture);

		int GetDrawOrder() const { return m_DrawOrder; }
		int GetTexHeight() const { return m_TexHeight; }
		int GetTexWidth() const { return m_TexWidth; }
	private:
		SDL_Texture* m_Texture;
		int m_DrawOrder;
		int m_TexWidth;
		int m_TexHeight;
	};
}