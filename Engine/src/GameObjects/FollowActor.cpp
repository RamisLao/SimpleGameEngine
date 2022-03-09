#include "FollowActor.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "FollowCamera.h"
#include "InputComponent.h"
#include "Mesh.h"

namespace Engine
{
	FollowActor::FollowActor(Game* game)
		:Actor(game)
	{
		m_MeshComp = new MeshComponent(this);
		m_MeshComp->SetMesh(game->GetRenderer()->GetMesh("src/Assets/3DGraphics/RacingCar.gpmesh"));
		SetPosition(Vector3(0.0f, 0.0f, -100.0f));

		m_InputComp = new InputComponent(this);
		m_InputComp->SetMass(1.0f);
		m_InputComp->SetMaxVelocity(500);
		m_InputComp->SetForwardForce(3000);
		m_InputComp->SetStrafeForce(3000);
		m_InputComp->SetAngularForce(CustomMath::Pi / 2);
		m_InputComp->SetForwardKey(SDL_SCANCODE_W);
		m_InputComp->SetBackKey(SDL_SCANCODE_S);
		m_InputComp->SetClockwiseKey(SDL_SCANCODE_D);
		m_InputComp->SetCounterClockwiseKey(SDL_SCANCODE_A);
		m_InputComp->SetUseAngularForce(true);
		m_CameraComp = new FollowCamera(this);
		m_CameraComp->SnapToIdeal();
	}

	void FollowActor::ActorInput(const struct InputState& state)
	{
		// Adjust horizontal distance of camera based on speed
		if (!CustomMath::NearZero(m_InputComp->GetCurrentVelocityLength()))
		{
			m_CameraComp->SetHorzDist(500.0f);
		}
		else
		{
			m_CameraComp->SetHorzDist(350.0f);
		}
	}

	void FollowActor::SetVisible(bool visible)
	{
		m_MeshComp->SetVisible(visible);
	}
}