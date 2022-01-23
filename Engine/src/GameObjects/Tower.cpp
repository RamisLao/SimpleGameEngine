
#include "Tower.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "Enemy.h"
#include "Bullet.h"

namespace Engine
{
	Tower::Tower(Game* game) :
		Actor(game)
	{
		SpriteComponent* sc = new SpriteComponent(this, 200);
		sc->SetTexture(game->GetTexture("src/Assets/TowerDefense/Tower.png"));

		m_NextAttack = AttackTime;
	}

	void Tower::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		m_NextAttack -= deltaTime;
		if (m_NextAttack <= 0.0f)
		{
			Enemy* e = GetGame()->GetNearestEnemy(GetPosition());
			if (e != nullptr)
			{
				Vector2 dir = e->GetPosition() - GetPosition();
				float dist = dir.Length();
				if (dist < AttackRange)
				{
					SetRotation(CustomMath::Atan2(-dir.y, dir.x));
					Bullet* b = new Bullet(GetGame());
					b->SetPosition(GetPosition());
					b->SetRotation(GetRotation());
				}
			}
			m_NextAttack += AttackTime;
		}
	}
}