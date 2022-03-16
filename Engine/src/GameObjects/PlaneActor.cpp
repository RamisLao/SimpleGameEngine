
#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BoxComponent.h"

namespace Engine
{
	PlaneActor::PlaneActor(Game* game) :
		Actor(game)
	{
		SetScale(10.0f);
		MeshComponent* mc = new MeshComponent(this);
		Mesh* mesh = GetGame()->GetRenderer()->GetMesh("src/Assets/3DGraphics/Plane.gpmesh");
		mc->SetMesh(mesh);

		m_Box = new BoxComponent(this);
		m_Box->SetObjectBox(mesh->GetBox());

		game->AddPlane(this);
	}

	PlaneActor::~PlaneActor()
	{
		GetGame()->RemovePlane(this);
	}
}