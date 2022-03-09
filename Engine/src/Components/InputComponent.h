#pragma once
#include "MoveComponent.h"
#include <cstdint>
#include "SDL_scancode.h"

namespace Engine
{
	class InputComponent : public MoveComponent
	{
	public:
		InputComponent(class Actor* owner);

		void ProcessInput(const struct InputState& state) override;

		// Getters/setters for private variables
		float GetForwardForce() const { return m_ForwardForce; }
		float GetAngularForce() const { return m_AngularForce; }
		float GetStrafeForce() const { return m_StrafeForce; }
		SDL_Scancode GetForwardKey() const { return m_ForwardKey; }
		SDL_Scancode GetBackKey() const { return m_BackKey; }
		SDL_Scancode GetClockwiseKey() const { return m_ClockwiseKey; }
		SDL_Scancode GetCounterClockwiseKey() const { return m_CounterClockwiseKey; }

		void SetForwardForce(float force) { m_ForwardForce = force; }
		void SetAngularForce(float force) { m_AngularForce = force; }
		void SetStrafeForce(float force) { m_StrafeForce = force; }
		void SetForwardKey(SDL_Scancode key) { m_ForwardKey = key; }
		void SetBackKey(SDL_Scancode key) { m_BackKey = key; }
		void SetClockwiseKey(SDL_Scancode key) { m_ClockwiseKey = key; }
		void SetCounterClockwiseKey(SDL_Scancode key) { m_CounterClockwiseKey = key; }
		void SetUseAngularForce(bool use) { m_UseAngularForce = use; }
	private:
		// The maximum forward/angular speeds
		float m_ForwardForce;
		float m_AngularForce;
		float m_StrafeForce;
		bool m_UseAngularForce;
		// Keys for forward/back movement
		SDL_Scancode m_ForwardKey;
		SDL_Scancode m_BackKey;
		// Keys for angular movement
		SDL_Scancode m_ClockwiseKey;
		SDL_Scancode m_CounterClockwiseKey;
	};
}