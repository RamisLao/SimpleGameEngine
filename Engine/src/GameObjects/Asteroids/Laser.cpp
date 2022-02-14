
#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"
#include "Texture.h"

namespace Engine
{
	Laser::Laser(Game* game) :
		Actor(game),
		m_DeathTimer(1.0f),
		m_Speed(100000)
	{
		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/Asteroids/Laser.png"));

		m_Move = new MoveComponent(this);
		m_Move->SetMass(0.01f);
		m_Move->SetMaxVelocity(800.0f);
		m_Move->SetScreenWrap(true);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(11.0f);
	}

	void Laser::UpdateActor(float deltaTime)
	{
		Vector3 force = GetForward() * m_Speed * deltaTime;
		m_Move->AddForce(force);

		m_DeathTimer -= deltaTime;
		if (m_DeathTimer <= 0.0f)
		{
			SetState(EDead);
		}
		else
		{
			for (auto ast : GetGame()->GetAsteroids())
			{
				if (CircleComponent::Intersect(*m_Circle, *(ast->GetCircle())))
				{
					SetState(EDead);
					ast->SetState(EDead);
					break;
				}
			}
		}
	}
}