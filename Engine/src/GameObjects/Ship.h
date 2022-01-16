#pragma once
#include "Actor.h"

namespace Engine
{
	class Ship : public Actor
	{
	public:
		Ship(class Game* game);
		void UpdateActor(float deltaTime) override;
		void ProcessKeyboard(const uint8_t* state);
		float GetRightSpeed() const { return m_RightSpeed; }
		float GetDownSpeed() const { return m_DownSpeed; }
	private:
		float m_RightSpeed;
		float m_DownSpeed;
	};
}