#pragma once
#include "Actor.h"

namespace Engine
{
	class Ship : public Actor
	{
	public:
		Ship(class Game* game);
		void UpdateActor(float deltaTime) override;
		void ActorInput(const struct InputState& state) override;
	private:
		float m_LaserCooldown;
		bool m_IsInvulnerable;
		float m_InvulnerabilityCooldown;
		class CircleComponent* m_Circle;
	};
}