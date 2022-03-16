
#include "TargetActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "Mesh.h"

namespace Engine
{
	TargetActor::TargetActor(Game* game)
		:Actor(game)
	{
		//SetScale(10.0f);
		SetRotation(Quaternion(Vector3::UnitZ, CustomMath::Pi));
		MeshComponent* mc = new MeshComponent(this);
		Mesh* mesh = GetGame()->GetRenderer()->GetMesh("src/Assets/3DGraphics/Target.gpmesh");
		mc->SetMesh(mesh);
		// Add collision box
		BoxComponent* bc = new BoxComponent(this);
		bc->SetObjectBox(mesh->GetBox());
	}
}

