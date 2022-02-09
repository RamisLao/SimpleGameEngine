
#include "Ship.h"
#include "SpriteComponent.h"
#include "AnimSpriteComponent.h"
#include "Game.h"
#include "InputComponent.h"
#include "Laser.h"
#include "CircleComponent.h"
#include "Asteroid.h"
#include "Texture.h"

namespace Engine
{
	Ship::Ship(Game* game) :
		Actor(game),
		m_LaserCooldown(0.0f),
		m_IsInvulnerable(false),
		m_InvulnerabilityCooldown(2.0),
		m_Circle(nullptr)
	{

		AnimSpriteComponent* asc = new AnimSpriteComponent(this, 150);
		std::vector<Texture*> anims = {
			game->GetTexture("src/Assets/AnimatedShip/Ship01.png"),
			game->GetTexture("src/Assets/AnimatedShip/Ship02.png"),
			game->GetTexture("src/Assets/AnimatedShip/Ship03.png"),
			game->GetTexture("src/Assets/AnimatedShip/Ship04.png")
		};

		asc->SetAnimTextures(anims);

		SetScale(1.5f);

		InputComponent* ic = new InputComponent(this);
		ic->SetForwardKey(SDL_SCANCODE_W);
		ic->SetBackKey(SDL_SCANCODE_S);
		ic->SetClockwiseKey(SDL_SCANCODE_A);
		ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
		ic->SetForwardForce(1000.0f);
		ic->SetAngularForce(CustomMath::TwoPi);
		ic->SetMass(1.5f);
		ic->SetMaxVelocity(300);
		ic->SetScreenWrap(true);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(6.0f);
	}

	void Ship::UpdateActor(float deltaTime)
	{
		m_LaserCooldown -= deltaTime;

		if (m_IsInvulnerable == true)
		{
			m_InvulnerabilityCooldown -= deltaTime;
			if (m_InvulnerabilityCooldown <= 0)
			{
				SetState(EActive);
				m_IsInvulnerable = false;
				m_InvulnerabilityCooldown = 2.0f;
			}

			return;
		}

		for (auto ast : GetGame()->GetAsteroids())
		{
			if (CircleComponent::Intersect(*m_Circle, *(ast->GetCircle())))
			{
				SetState(EInvisible);
				m_IsInvulnerable = true;
				SetPosition(Vector2::Zero);
				SetRotation(CustomMath::PiOver2);
				ast->SetState(EDead);
				break;
			}
		}
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