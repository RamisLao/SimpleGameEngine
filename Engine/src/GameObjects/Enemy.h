#pragma once
#include "Actor.h"

namespace Engine
{
	class Enemy : public Actor
	{
	public:
		Enemy(class Game* game);
		~Enemy();
		void UpdateActor(float deltaTime) override;
		class CircleComponent* GetCircle() { return m_Circle; }
	private:
		class CircleComponent* m_Circle;
	};
}
