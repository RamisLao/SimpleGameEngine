#pragma once
#include "MoveComponent.h"

namespace Engine
{
	class BallMove : public MoveComponent
	{
	public:
		BallMove(class Actor* owner);

		void SetPlayer(Actor* player) { m_Player = player; }
		void Update(float deltaTime) override;
	protected:
		class Actor* m_Player;
	};
}
