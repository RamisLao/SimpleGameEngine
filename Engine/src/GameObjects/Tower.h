#pragma once
#include "Actor.h"

namespace Engine
{
	class Tower : public Actor
	{
	public:
		Tower(class Game* game);
		void UpdateActor(float deltaTime) override;
	private:
		class MoveComponent* m_Move;
		float m_NextAttack;
		const float AttackTime = 2.5f;
		const float AttackRange = 100.0f;
	};
}
