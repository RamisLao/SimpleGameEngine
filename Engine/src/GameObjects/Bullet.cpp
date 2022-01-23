
#include "Bullet.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleComponent.h"
#include "Enemy.h"

namespace Engine
{
	Bullet::Bullet(Game* game) :
		Actor(game)
	{
		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/TowerDeffense/Projectile.png"));

		MoveComponent* mc = new MoveComponent(this);
		mc->SetForwardSpeed(400.0f);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(5.0f);

		m_LiveTime = 1.0f;
	}

	void Bullet::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Check for collision vs enemies
		for (Enemy* e : GetGame()->GetEnemies())
		{
			if (CircleComponent::Intersect(*m_Circle, *(e->GetCircle())))
			{
				// We both die on collision
				e->SetState(EDead);
				SetState(EDead);
				break;
			}
		}

		m_LiveTime -= deltaTime;
		if (m_LiveTime <= 0.0f)
		{
			// Time limit hit, die
			SetState(EDead);
		}
	}
}