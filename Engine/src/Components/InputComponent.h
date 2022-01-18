#pragma once
#include "MoveComponent.h"
#include <cstdint>

namespace Engine
{
	class InputComponent : public MoveComponent
	{
	public:
		InputComponent(class Actor* owner);

		void ProcessInput(const uint8_t* keyState) override;

		// Getters/setters for private variables
		float GetMaxForward() const { return m_MaxForwardSpeed; }
		float GetMaxAngular() const { return m_MaxAngularSpeed; }
		int GetForwardKey() const { return m_ForwardKey; }
		int GetBackKey() const { return m_BackKey; }
		int GetClockwiseKey() const { return m_ClockwiseKey; }
		int GetCounterClockwiseKey() const { return m_CounterClockwiseKey; }

		void SetMaxForwardSpeed(float speed) { m_MaxForwardSpeed = speed; }
		void SetMaxAngularSpeed(float speed) { m_MaxAngularSpeed = speed; }
		void SetForwardKey(int key) { m_ForwardKey = key; }
		void SetBackKey(int key) { m_BackKey = key; }
		void SetClockwiseKey(int key) { m_ClockwiseKey = key; }
		void SetCounterClockwiseKey(int key) { m_CounterClockwiseKey = key; }
	private:
		// The maximum forward/angular speeds
		float m_MaxForwardSpeed;
		float m_MaxAngularSpeed;
		// Keys for forward/back movement
		int m_ForwardKey;
		int m_BackKey;
		// Keys for angular movement
		int m_ClockwiseKey;
		int m_CounterClockwiseKey;
	};
}