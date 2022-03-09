
#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "Mesh.h"

namespace Engine
{
	FPSActor::FPSActor(Game* game)
		:Actor(game)
	{
		m_MoveComp = new MoveComponent(this);
		m_AudioComp = new AudioComponent(this);
		m_LastFootstep = 0.0f;
		m_Footstep = m_AudioComp->PlayEvent("event:/Footstep");
		m_Footstep.SetPaused(true);

		m_CameraComp = new FPSCamera(this);

		m_FPSModel = new Actor(game);
		m_FPSModel->SetScale(0.75f);
		m_MeshComp = new MeshComponent(m_FPSModel);
		m_MeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));
	}

	void FPSActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Play the footstep if we're moving and haven't recently
		mLastFootstep -= deltaTime;
		if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
		{
			mFootstep.SetPaused(false);
			mFootstep.Restart();
			mLastFootstep = 0.5f;
		}

		// Update position of FPS model relative to actor position
		const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
		Vector3 modelPos = GetPosition();
		modelPos += GetForward() * modelOffset.x;
		modelPos += GetRight() * modelOffset.y;
		modelPos.z += modelOffset.z;
		mFPSModel->SetPosition(modelPos);
		// Initialize rotation to actor rotation
		Quaternion q = GetRotation();
		// Rotate by pitch from camera
		q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
		mFPSModel->SetRotation(q);
	}

	void FPSActor::ActorInput(const uint8_t* keys)
	{
		float forwardSpeed = 0.0f;
		float strafeSpeed = 0.0f;
		// wasd movement
		if (keys[SDL_SCANCODE_W])
		{
			forwardSpeed += 400.0f;
		}
		if (keys[SDL_SCANCODE_S])
		{
			forwardSpeed -= 400.0f;
		}
		if (keys[SDL_SCANCODE_A])
		{
			strafeSpeed -= 400.0f;
		}
		if (keys[SDL_SCANCODE_D])
		{
			strafeSpeed += 400.0f;
		}

		mMoveComp->SetForwardSpeed(forwardSpeed);
		mMoveComp->SetStrafeSpeed(strafeSpeed);

		// Mouse movement
		// Get relative movement from SDL
		int x, y;
		SDL_GetRelativeMouseState(&x, &y);
		// Assume mouse movement is usually between -500 and +500
		const int maxMouseSpeed = 500;
		// Rotation/sec at maximum speed
		const float maxAngularSpeed = Math::Pi * 8;
		float angularSpeed = 0.0f;
		if (x != 0)
		{
			// Convert to ~[-1.0, 1.0]
			angularSpeed = static_cast<float>(x) / maxMouseSpeed;
			// Multiply by rotation/sec
			angularSpeed *= maxAngularSpeed;
		}
		mMoveComp->SetAngularSpeed(angularSpeed);

		// Compute pitch
		const float maxPitchSpeed = Math::Pi * 8;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// Convert to ~[-1.0, 1.0]
			pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
			pitchSpeed *= maxPitchSpeed;
		}
		mCameraComp->SetPitchSpeed(pitchSpeed);
	}

	void FPSActor::SetFootstepSurface(float value)
	{
		// Pause here because the way I setup the parameter in FMOD
		// changing it will play a footstep
		mFootstep.SetPaused(true);
		mFootstep.SetParameter("Surface", value);
	}

	void FPSActor::SetVisible(bool visible)
	{
		mMeshComp->SetVisible(visible);
	}
}