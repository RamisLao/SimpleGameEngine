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
	private:
		class CircleComponent* m_Circle;
	};
}