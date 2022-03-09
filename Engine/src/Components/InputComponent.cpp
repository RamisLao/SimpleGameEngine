
#include "InputComponent.h"
#include "Actor.h"
#include "InputSystem.h"

namespace Engine
{
	InputComponent::InputComponent(Actor* owner)
		: MoveComponent(owner),
		m_ForwardKey(SDL_SCANCODE_0),
		m_BackKey(SDL_SCANCODE_0),
		m_ClockwiseKey(SDL_SCANCODE_0),
		m_CounterClockwiseKey(SDL_SCANCODE_0),
		m_AngularForce(0),
		m_ForwardForce(0)
	{
	}

	void InputComponent::ProcessInput(const struct InputState& state)
	{
		if (state.Keyboard.GetKeyState(m_ForwardKey) == EPressed ||
			state.Keyboard.GetKeyState(m_ForwardKey) == EHeld)
		{
			Vector3 force = m_Owner->GetForward() * m_ForwardForce;
			AddForce(force);
		}

		if (state.Keyboard.GetKeyState(m_BackKey) == EPressed ||
			state.Keyboard.GetKeyState(m_BackKey) == EHeld)
		{
			Vector3 force = m_Owner->GetForward() * -1 * m_ForwardForce;
			AddForce(force);
		}

		float angularSpeed = 0.0f;
		if (state.Keyboard.GetKeyState(m_ClockwiseKey) == EPressed ||
			state.Keyboard.GetKeyState(m_ClockwiseKey) == EHeld)
		{
			Vector3 force = m_Owner->GetRight() * m_StrafeForce;
			AddForce(force);
		}
		if (state.Keyboard.GetKeyState(m_CounterClockwiseKey) == EPressed ||
			state.Keyboard.GetKeyState(m_CounterClockwiseKey) == EHeld)
		{
			Vector3 force = m_Owner->GetRight() * -1 * m_StrafeForce;
			AddForce(force);
		}
		SetAngularSpeed(angularSpeed);
	}
}