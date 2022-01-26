
#include "Enemy.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "NavComponent.h"
#include "CircleComponent.h"
#include "Grid.h"
#include "Tile.h"

namespace Engine
{
	Enemy::Enemy(Game* game) :
		Actor(game)
	{
		game->GetEnemies().emplace_back(this);

		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/TowerDefense/Airplane.png"));

		SetPosition(GetGame()->GetGrid()->GetStartTile()->GetPosition());

		NavComponent* nc = new NavComponent(this);
		nc->SetMass(0.1f);
		nc->SetMaxVelocity(150.0f);
		nc->SetForwardSpeed(100000.0f);
		nc->SetDrag(1.0f);
		nc->StartPath(GetGame()->GetGrid()->GetStartTile());

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(25.0f);
	}

	Enemy::~Enemy()
	{
		// Remove from enemy vector
		auto iter = std::find(GetGame()->GetEnemies().begin(),
			GetGame()->GetEnemies().end(),
			this);
		GetGame()->GetEnemies().erase(iter);
	}

	void Enemy::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Am I near the end tile?
		Vector2 diff = GetPosition() - GetGame()->GetGrid()->GetEndTile()->GetPosition();
		if (CustomMath::NearZero(diff.Length(), 10.0f))
		{
			SetState(EDead);
		}
	}
}