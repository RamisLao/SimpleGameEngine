#pragma once
#include "Component.h"

namespace Engine
{
	class MoveComponent : public Component
	{
	public:
		// Lower update order to update first
		MoveComponent(class Actor* owner, int updateOrder = 10);

		void Update(float deltaTime) override;

		float GetAngularSpeed() const { return m_AngularSpeed; }
		float GetForwardSpeed() const { return m_ForwardSpeed; }
		void SetAngularSpeed(float speed) { m_AngularSpeed = speed; }
		void SetForwardSpeed(float speed) { m_ForwardSpeed = speed; }
	private:
		// Controls rotation (radians / second)
		float m_AngularSpeed;
		// Controls forward movement (units / second)
		float m_ForwardSpeed;
	};
}