
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
#include "BallActor.h"
#include "BoxComponent.h"
#include "PlaneActor.h"

namespace Engine
{
	FPSActor::FPSActor(Game* game)
		:Actor(game)
	{
		m_InputComp = new InputComponent(this);
		m_InputComp->SetMass(1.0f);
		m_InputComp->SetMaxVelocity(500);
		m_InputComp->SetForwardForce(2000);
		m_InputComp->SetStrafeForce(2000);
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

		// Add a box component
		m_BoxComp = new BoxComponent(this);
		AABB myBox(Vector3(-25.0f, -25.0f, -87.5f),
			Vector3(25.0f, 25.0f, 87.5f));
		m_BoxComp->SetObjectBox(myBox);
		m_BoxComp->SetShouldRotate(false);
	}

	void FPSActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		FixCollisions();

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

		if (state.Mouse.GetButtonState(SDL_BUTTON_LEFT) == EPressed)
		{
			Shoot();
		}
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

	void FPSActor::Shoot()
	{
		Vector3 start;
		Vector3 dir;
		GetGame()->GetRenderer()->GetScreenDirection(start, dir);
		// Spawn a ball
		BallActor* ball = new BallActor(GetGame());
		ball->SetPlayer(this);
		ball->SetPosition(start + dir * 20.0f);
		// Rotate the ball to face new direction
		ball->RotateToNewForward(dir);
		// Play shooting sound
		m_AudioComp->PlayEvent("event:/Shot");
	}

	void FPSActor::FixCollisions()
	{
		// Need to recompute my world transform to update world box
		ComputeWorldTransform();

		const AABB& playerBox = m_BoxComp->GetWorldBox();
		Vector3 pos = GetPosition();

		auto& planes = GetGame()->GetPlanes();
		for (auto pa : planes)
		{
			// Do we collide with this PlaneActor?
			const AABB& planeBox = pa->GetBox()->GetWorldBox();
			if (Intersect(playerBox, planeBox))
			{
				// Calculate all our differences
				float dx1 = planeBox.m_Max.x - playerBox.m_Min.x;
				float dx2 = planeBox.m_Min.x - playerBox.m_Max.x;
				float dy1 = planeBox.m_Max.y - playerBox.m_Min.y;
				float dy2 = planeBox.m_Min.y - playerBox.m_Max.y;
				float dz1 = planeBox.m_Max.z - playerBox.m_Min.z;
				float dz2 = planeBox.m_Min.z - playerBox.m_Max.z;

				// Set dx to whichever of dx1/dx2 have a lower abs
				float dx = CustomMath::Abs(dx1) < CustomMath::Abs(dx2) ?
					dx1 : dx2;
				// Ditto for dy
				float dy = CustomMath::Abs(dy1) < CustomMath::Abs(dy2) ?
					dy1 : dy2;
				// Ditto for dz
				float dz = CustomMath::Abs(dz1) < CustomMath::Abs(dz2) ?
					dz1 : dz2;

				// Whichever is closest, adjust x/y position
				if (CustomMath::Abs(dx) <= CustomMath::Abs(dy) && CustomMath::Abs(dx) <= CustomMath::Abs(dz))
				{
					pos.x += dx;
				}
				else if (CustomMath::Abs(dy) <= CustomMath::Abs(dx) && CustomMath::Abs(dy) <= CustomMath::Abs(dz))
				{
					pos.y += dy;
				}
				else
				{
					pos.z += dz;
				}

				// Need to set position and update box component
				SetPosition(pos);
				m_BoxComp->OnUpdateWorldTransform();
			}
		}
	}
}