
#include "BallActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BallMove.h"
#include "AudioComponent.h"

namespace Engine
{
	BallActor::BallActor(Game* game)
		:Actor(game)
		, m_LifeSpan(2.0f)
	{
		//SetScale(10.0f);
		MeshComponent* mc = new MeshComponent(this);
		Mesh* mesh = GetGame()->GetRenderer()->GetMesh("src/Assets/3DGraphics/Sphere.gpmesh");
		mc->SetMesh(mesh);
		m_MyMove = new BallMove(this);
		m_MyMove->SetForwardSpeed(400000.0f);
		m_MyMove->SetMaxVelocity(10000);
		m_MyMove->SetMass(0.1f);
		m_MyMove->SetDrag(0.1f);
		m_AudioComp = new AudioComponent(this);
	}

	void BallActor::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);

		m_LifeSpan -= deltaTime;
		if (m_LifeSpan < 0.0f)
		{
			SetState(EDead);
		}
	}

	void BallActor::SetPlayer(Actor* player)
	{
		m_MyMove->SetPlayer(player);
	}

	void BallActor::HitTarget()
	{
		m_AudioComp->PlayEvent("event:/Ding");
	}
}