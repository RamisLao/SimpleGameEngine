
#include "FPSCamera.h"
#include "Actor.h"

namespace Engine
{
	FPSCamera::FPSCamera(Actor* owner)
		:CameraComponent(owner)
		, m_PitchSpeed(0.0f)
		, m_MaxPitch(CustomMath::Pi / 3.0f)
		, m_Pitch(0.0f)
	{
	}

	void FPSCamera::Update(float deltaTime)
	{
		// Call parent update (doesn't do anything right now)
		CameraComponent::Update(deltaTime);
		// Camera position is owner position
		Vector3 cameraPos = m_Owner->GetPosition();

		// Update pitch based on pitch speed
		m_Pitch += m_PitchSpeed * deltaTime;
		// Clamp pitch to [-max, +max]
		m_Pitch = CustomMath::Clamp(m_Pitch, -m_MaxPitch, m_MaxPitch);
		// Make a quaternion representing pitch rotation,
		// which is about owner's right vector
		Quaternion q(m_Owner->GetRight(), m_Pitch);

		// Rotate owner forward by pitch quaternion
		Vector3 viewForward = Vector3::Transform(
			m_Owner->GetForward(), q);
		// Target position 100 units in front of view forward
		Vector3 target = cameraPos + viewForward * 100.0f;
		// Also rotate up by pitch quaternion
		Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

		// Create look at matrix, set as view
		Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
		SetViewMatrix(view);
	}
}