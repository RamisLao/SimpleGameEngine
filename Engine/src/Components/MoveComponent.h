#pragma once
#include "Component.h"
#include "CustomMath.h"

namespace Engine
{
	class MoveComponent : public Component
	{
	public:
		// Lower update order to update first
		MoveComponent(class Actor* owner, int updateOrder = 10);

		void Update(float deltaTime) override;

		void AddForce(Vector2& force);

		void SetAngularSpeed(float speed) { m_AngularSpeed = speed; }
		void SetMass(float mass) { m_Mass = mass; }
		void SetMaxVelocity(float maxVelocity) { m_MaxVelocity = maxVelocity; }
	private:
		// Controls rotation (radians / second)
		float m_AngularSpeed;
		float m_Mass;
		float m_Drag;
		float m_MaxVelocity;
		Vector2 m_SumOfForces;
		Vector2 m_Velocity;
	};
}