#pragma once
#include "Actor.h"

namespace Engine
{
	class Asteroid : public Actor
	{
	public:
		Asteroid(class Game* game);
		~Asteroid();

		class CircleComponent* GetCircle() { return m_Circle; }
		void UpdateActor(float deltaTime) override;
	private:
		class MoveComponent* m_Move;
		class CircleComponent* m_Circle;
		float m_Speed;
	};
}