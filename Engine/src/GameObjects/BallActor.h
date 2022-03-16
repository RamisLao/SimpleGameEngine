#pragma once
#include "Actor.h"

namespace Engine
{
	class BallActor : public Actor
	{
	public:
		BallActor(class Game* game);

		void UpdateActor(float deltaTime) override;
		void SetPlayer(Actor* player);

		void HitTarget();
	private:
		class AudioComponent* m_AudioComp;
		class BallMove* m_MyMove;
		float m_LifeSpan;
	};
}

