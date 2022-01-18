
#include "InputComponent.h"

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
			forwardSpeed += m_MaxForwardSpeed;
		}
		if (keyState[m_BackKey])
		{
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