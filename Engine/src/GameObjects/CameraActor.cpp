
#include "CameraActor.h"
#include "MoveComponent.h"
#include "InputComponent.h"
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
		m_InputComp = new InputComponent(this);
		m_InputComp->SetMass(1.0f);
		m_InputComp->SetMaxVelocity(200);
		m_InputComp->SetForwardForce(1000);
		m_InputComp->SetAngularForce(CustomMath::Pi);
		m_InputComp->SetForwardKey(SDL_SCANCODE_W);
		m_InputComp->SetBackKey(SDL_SCANCODE_S);
		m_InputComp->SetClockwiseKey(SDL_SCANCODE_D);
		m_InputComp->SetCounterClockwiseKey(SDL_SCANCODE_A);
		m_AudioComp = new AudioComponent(this);
		m_LastFootstep = 0.0f;
		m_Footstep = m_AudioComp->PlayEvent("event:/Footstep");
		m_Footstep.SetPaused(true);
	}

	void CameraActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		// Play the footstep if we're moving and haven't recently
		m_LastFootstep -= deltaTime;
		if (!CustomMath::NearZero(m_InputComp->GetForwardSpeed()) && m_LastFootstep <= 0.0f)
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

	void CameraActor::SetFootstepSurface(float value)
	{
		// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
		m_Footstep.SetPaused(true);
		m_Footstep.SetParameter("Surface", value);
	}
}