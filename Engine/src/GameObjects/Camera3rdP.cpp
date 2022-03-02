// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Camera3rdP.h"
#include "MoveComponent.h"
#include "SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "MeshComponent.h"
#include "Mesh.h"

namespace Engine
{
	Camera3rdP::Camera3rdP(Game* game)
		:Actor(game)
	{
		m_MoveComp = new MoveComponent(this);
		m_MoveComp->SetMass(1.0f);
		m_MoveComp->SetMaxVelocity(10000);
		m_AudioComp = new AudioComponent(this);
		MeshComponent* mc = new MeshComponent(this);
		mc->SetMesh(game->GetRenderer()->GetMesh("src/Assets/3DGraphics/Sphere.gpmesh"));
		m_LastFootstep = 0.0f;
		m_Footstep = m_AudioComp->PlayEvent("event:/Footstep");
		m_Footstep.SetPaused(true);
	}

	void Camera3rdP::UpdateActor(float deltaTime)
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

		// Compute new camera from this actor
		m_CameraPos = GetPosition() - GetForward() * 200.0f + Vector3::UnitZ * 100.0f;
		Vector3 target = GetPosition() + GetForward() * 100.0f;
		Vector3 up = Vector3::UnitZ;
		Matrix4 view = Matrix4::CreateLookAt(m_CameraPos, target, up);
		GetGame()->GetRenderer()->SetViewMatrix(view);
		GetGame()->GetAudioSystem()->SetListener(view);
	}

	void Camera3rdP::ActorInput(const uint8_t* keys)
	{
		float forwardSpeed = 0.0f;
		float angularSpeed = 0.0f;
		// wasd movement
		if (keys[SDL_SCANCODE_W])
		{
			forwardSpeed += 50000.0f;
		}
		if (keys[SDL_SCANCODE_S])
		{
			forwardSpeed -= 50000.0f;
		}
		if (keys[SDL_SCANCODE_A])
		{
			angularSpeed -= CustomMath::TwoPi;
		}
		if (keys[SDL_SCANCODE_D])
		{
			angularSpeed += CustomMath::TwoPi;
		}

		m_MoveComp->SetForwardSpeed(forwardSpeed);
		m_MoveComp->SetAngularSpeed(angularSpeed);
	}

	void Camera3rdP::SetFootstepSurface(float value)
	{
		// Pause here because the way I setup the parameter in FMOD
		// changing it will play a footstep
		m_Footstep.SetPaused(true);
		m_Footstep.SetParameter("Surface", value);
	}
}
