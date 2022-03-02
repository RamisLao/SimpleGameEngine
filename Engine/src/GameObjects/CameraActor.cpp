
#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL_scancode.h"
#include "Renderer.h"
#include "Game.h"
#include "AudioSystem.h"
#include "AudioComponent.h"

namespace Engine
{
	CameraActor::CameraActor(Game* game) :
		Actor(game)
	{
		m_MoveComp = new MoveComponent(this);
		m_AudioComp = new AudioComponent(this);
		m_MoveComp->SetMass(1.0f);
		m_MoveComp->SetMaxVelocity(10000);
		m_LastFootstep = 0.0f;
		m_Footstep = m_AudioComp->PlayEvent("event:/Footstep");
		m_Footstep.SetPaused(true);
	}

	void CameraActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Play the footstep if we're moving and haven't recently
		m_LastFootstep -= deltaTime;
		if (!CustomMath::NearZero(m_MoveComp->GetForwardSpeed()) && m_LastFootstep <= 0.0f)
		{
			m_Footstep.SetPaused(false);
			m_Footstep.Restart();
			m_LastFootstep = 0.5f;
		}

		Vector3 cameraPos = GetPosition();
		Vector3 target = GetPosition() + GetForward() * 10.0f;
		Vector3 up = Vector3::UnitZ;

		Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
		GetGame()->GetRenderer()->SetViewMatrix(view);
		GetGame()->GetAudioSystem()->SetListener(view);
	}

	void CameraActor::ActorInput(const uint8_t* keys)
	{
		float forwardSpeed = 0.0f;
		float angularSpeed = 0.0f;

		if (keys[SDL_SCANCODE_W])
		{
			forwardSpeed += 50000.f;
		}
		if (keys[SDL_SCANCODE_S])
		{
			forwardSpeed -= 50000.f;
		}
		if (keys[SDL_SCANCODE_A])
		{
			angularSpeed -= CustomMath::Pi;
		}
		if (keys[SDL_SCANCODE_D])
		{
			angularSpeed += CustomMath::Pi;
		}

		m_MoveComp->SetForwardSpeed(forwardSpeed);
		m_MoveComp->SetAngularSpeed(angularSpeed);
	}

	void CameraActor::SetFootstepSurface(float value)
	{
		// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
		m_Footstep.SetPaused(true);
		m_Footstep.SetParameter("Surface", value);
	}
}