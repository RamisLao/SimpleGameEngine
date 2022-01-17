#pragma once
#include "Actor.h"

namespace Engine
{
	class Asteroid : public Actor
	{
	public:
		Asteroid(class Game* game);
		~Asteroid();
	};
}