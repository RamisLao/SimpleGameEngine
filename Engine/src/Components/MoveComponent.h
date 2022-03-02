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

		void AddForce(Vector3& force);
		void ResetVelocity() { m_Velocity = Vector3::Zero; }

		float GetForwardSpeed() const { return m_ForwardSpeed; }

		void SetScreenWrap(bool screenWrap) { m_ScreenWrap = screenWrap; }
		void SetAngularSpeed(float speed) { m_AngularSpeed = speed; }
		void SetForwardSpeed(float speed) { m_ForwardSpeed = speed; }
		void SetStatic(bool staticBool) { m_Static = staticBool; }
		void SetMass(float mass) { m_Mass = mass; }
		void SetDrag(float drag) { m_Drag = drag; }
		void SetMaxVelocity(float maxVelocity) { m_MaxVelocity = maxVelocity; }
	private:
		bool m_ScreenWrap;
		// Controls rotation (radians / second)
		float m_AngularSpeed;
		float m_ForwardSpeed;
		// If m_Static is true, the object can only rotate but not move
		bool m_Static;
		float m_Mass;
		float m_Drag;
		float m_MaxVelocity;
		Vector3 m_SumOfForces;
		Vector3 m_Velocity;
	};
}