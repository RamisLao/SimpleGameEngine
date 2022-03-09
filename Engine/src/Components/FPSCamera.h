#pragma once
#include "CameraComponent.h"

namespace Engine
{
	class FPSCamera : public CameraComponent
	{
	public:
		FPSCamera(class Actor* owner);

		void Update(float deltaTime) override;

		float GetPitch() const { return m_Pitch; }
		float GetPitchSpeed() const { return m_PitchSpeed; }
		float GetMaxPitch() const { return m_MaxPitch; }

		void SetPitchSpeed(float speed) { m_PitchSpeed = speed; }
		void SetMaxPitch(float pitch) { m_MaxPitch = pitch; }
	private:
		// Rotation/sec speed of pitch
		float m_PitchSpeed;
		// Maximum pitch deviation from forward
		float m_MaxPitch;
		// Current pitch
		float m_Pitch;
	};
}