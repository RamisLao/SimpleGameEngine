
#include "Ship.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

namespace Engine
{
	Ship::Ship(Game* game) :
		Actor(game),
		m_RightSpeed(0.0f),
		m_DownSpeed(0.0f)
	{
		AnimSpriteComponent* asc = new AnimSpriteComponent(this);
		std::vector<SDL_Texture*> anims = {
			game->GetTexture("src/Assets/Ship01.png"),
			game->GetTexture("src/Assets/Ship02.png"),
			game->GetTexture("src/Assets/Ship03.png"),
			game->GetTexture("src/Assets/Ship04.png")
		};

		asc->SetAnimTextures(anims);
	}

	void Ship::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);
		Vector2 pos = GetPosition();
		pos.x += m_RightSpeed * deltaTime;
		pos.y += m_DownSpeed * deltaTime;

		if (pos.x < 25.0f)
		{
			pos.x = 25.0f;
		}
		else if (pos.x > 500.0f)
		{
			pos.x = 500.0f;
		}

		if (pos.y < 25.0f)
		{
			pos.y = 25.0f;
		}
		else if (pos.y > 743.0)
		{
			pos.y = 743.0f;
		}
		SetPosition(pos);
	}

	void Ship::ProcessKeyboard(const uint8_t* state)
	{
		m_RightSpeed = 0.0f;
		m_DownSpeed = 0.0f;

		if (state[SDL_SCANCODE_D])
		{
			m_RightSpeed += 250.0f;
		}

		if (state[SDL_SCANCODE_A])
		{
			m_RightSpeed -= 250.0f;
		}

		if (state[SDL_SCANCODE_S])
		{
			m_DownSpeed += 300.0f;
		}

		if (state[SDL_SCANCODE_W])
		{
			m_DownSpeed -= 300.0f;
		}
	}
}