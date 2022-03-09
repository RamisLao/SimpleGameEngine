
#include "FPSActor.h"
#include "InputComponent.h"
#include "SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "InputSystem.h"

namespace Engine
{
	FPSActor::FPSActor(Game* game)
		:Actor(game)
	{
		m_InputComp = new InputComponent(this);
		m_InputComp->SetMass(1.0f);
		m_InputComp->SetMaxVelocity(200);
		m_InputComp->SetForwardForce(1000);
		m_InputComp->SetStrafeForce(1000);
		m_InputComp->SetAngularForce(CustomMath::Pi);
		m_InputComp->SetForwardKey(SDL_SCANCODE_W);
		m_InputComp->SetBackKey(SDL_SCANCODE_S);
		m_InputComp->SetClockwiseKey(SDL_SCANCODE_D);
		m_InputComp->SetCounterClockwiseKey(SDL_SCANCODE_A);
		m_InputComp->SetUseAngularForce(false);
		m_AudioComp = new AudioComponent(this);
		m_LastFootstep = 0.0f;
		m_Footstep = m_AudioComp->PlayEvent("event:/Footstep");
		m_Footstep.SetPaused(true);

		m_CameraComp = new FPSCamera(this);

		m_FPSModel = new Actor(game);
		m_FPSModel->SetScale(0.75f);
		m_MeshComp = new MeshComponent(m_FPSModel);
		m_MeshComp->SetMesh(game->GetRenderer()->GetMesh("src/Assets/3DGraphics/Rifle.gpmesh"));
	}

	void FPSActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Play the footstep if we're moving and haven't recently
		/*m_LastFootstep -= deltaTime;
		if (!CustomMath::NearZero(m_MoveComp->GetForwardSpeed()) && m_LastFootstep <= 0.0f)
		{
			m_Footstep.SetPaused(false);
			m_Footstep.Restart();
			m_LastFootstep = 0.5f;
		}*/

		// Update position of FPS model relative to actor position
		const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
		Vector3 modelPos = GetPosition();
		modelPos += GetForward() * modelOffset.x;
		modelPos += GetRight() * modelOffset.y;
		modelPos.z += modelOffset.z;
		m_FPSModel->SetPosition(modelPos);
		// Initialize rotation to actor rotation
		Quaternion q = GetRotation();
		// Rotate by pitch from camera
		q = Quaternion::Concatenate(q, Quaternion(GetRight(), m_CameraComp->GetPitch()));
		m_FPSModel->SetRotation(q);
	}

	void FPSActor::ActorInput(const struct InputState& state)
	{
		// Mouse movement
		// Get relative movement from SDL
		Vector2 mousePosition = state.Mouse.GetPosition();
		
		// Assume mouse movement is usually between -500 and +500
		const int maxMouseSpeed = 500;
		// Rotation/sec at maximum speed
		const float maxAngularSpeed = CustomMath::Pi * 8;
		float angularSpeed = 0.0f;
		if (mousePosition.x != 0)
		{
			// Convert to ~[-1.0, 1.0]
			angularSpeed = mousePosition.x / maxMouseSpeed;
			// Multiply by rotation/sec
			angularSpeed *= maxAngularSpeed;
		}
		m_InputComp->SetAngularSpeed(angularSpeed);

		// Compute pitch
		const float maxPitchSpeed = CustomMath::Pi * 8;
		float pitchSpeed = 0.0f;
		if (mousePosition.y != 0)
		{
			// Convert to ~[-1.0, 1.0]
			pitchSpeed = mousePosition.y / maxMouseSpeed;
			pitchSpeed *= maxPitchSpeed;
		}
		m_CameraComp->SetPitchSpeed(pitchSpeed);
	}

	void FPSActor::SetFootstepSurface(float value)
	{
		// Pause here because the way I setup the parameter in FMOD
		// changing it will play a footstep
		m_Footstep.SetPaused(true);
		m_Footstep.SetParameter("Surface", value);
	}

	void FPSActor::SetVisible(bool visible)
	{
		m_MeshComp->SetVisible(visible);
	}
}