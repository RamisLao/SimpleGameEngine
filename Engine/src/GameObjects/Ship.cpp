
#include "Ship.h"
#include "AnimSpriteComponent.h"
#include "Game.h"
#include "InputComponent.h"
#include "Laser.h"

namespace Engine
{
	Ship::Ship(Game* game) :
		Actor(game),
		m_LaserCooldown(0.0f)
	{
		AnimSpriteComponent* asc = new AnimSpriteComponent(this, 150);
		std::vector<SDL_Texture*> anims = {
			game->GetTexture("src/Assets/Ship01.png"),
			game->GetTexture("src/Assets/Ship02.png"),
			game->GetTexture("src/Assets/Ship03.png"),
			game->GetTexture("src/Assets/Ship04.png")
		};

		asc->SetAnimTextures(anims);

		InputComponent* ic = new InputComponent(this);
		ic->SetForwardKey(SDL_SCANCODE_W);
		ic->SetBackKey(SDL_SCANCODE_S);
		ic->SetClockwiseKey(SDL_SCANCODE_A);
		ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
		ic->SetMaxForwardSpeed(300.0f);
		ic->SetMaxAngularSpeed(CustomMath::TwoPi);
	}

	void Ship::UpdateActor(float deltaTime)
	{
		m_LaserCooldown -= deltaTime;
	}

	void Ship::ActorInput(const uint8_t* keyState)
	{
		if (keyState[SDL_SCANCODE_SPACE] && m_LaserCooldown <= 0.0f)
		{
			Laser* laser = new Laser(GetGame());
			laser->SetPosition(GetPosition());
			laser->SetRotation(GetRotation());

			m_LaserCooldown = 0.5f;
		}
	}
}