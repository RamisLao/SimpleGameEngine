
#include "InputComponent.h"
#include "Actor.h"

namespace Engine
{
	InputComponent::InputComponent(Actor* owner)
		: MoveComponent(owner),
		m_ForwardKey(0),
		m_BackKey(0),
		m_ClockwiseKey(0),
		m_CounterClockwiseKey(0)
	{
	}

	void InputComponent::ProcessInput(const uint8_t* keyState)
	{
		float forwardSpeed = 0.0f;
		if (keyState[m_ForwardKey])
		{
			/*Vector2 force = m_Owner->GetForward() * m_MaxForwardSpeed;
			AddForce(force);*/
			forwardSpeed += m_MaxForwardSpeed;
		}
		if (keyState[m_BackKey])
		{
			/*Vector2 force = m_Owner->GetForward() * -1 * m_MaxForwardSpeed;
			AddForce(force);*/
			forwardSpeed -= m_MaxForwardSpeed;
		}
		SetForwardSpeed(forwardSpeed);

		float angularSpeed = 0.0f;
		if (keyState[m_ClockwiseKey])
		{
			angularSpeed += m_MaxAngularSpeed;
		}
		if (keyState[m_CounterClockwiseKey])
		{
			angularSpeed -= m_MaxAngularSpeed;
		}
		SetAngularSpeed(angularSpeed);
	}
}