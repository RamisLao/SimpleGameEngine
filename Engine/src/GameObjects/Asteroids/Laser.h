#pragma once
#include "Actor.h"

namespace Engine
{
	class Laser : public Actor
	{
	public:
		Laser(class Game* game);

		void UpdateActor(float deltaTime) override;
	private:
		class MoveComponent* m_Move;
		class CircleComponent* m_Circle;
		float m_DeathTimer;
		float m_Speed;
	};
}
