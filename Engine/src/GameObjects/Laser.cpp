
#include "Laser.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"

namespace Engine
{
	Laser::Laser(Game* game) :
		Actor(game),
		m_DeathTimer(1.0f)
	{
		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/Asteroids/Laser.png"));

		MoveComponent* mc = new MoveComponent(this);
		mc->SetForwardSpeed(800.0f);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(11.0f);
	}

	void Laser::UpdateActor(float deltaTime)
	{
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