
#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"

namespace Engine
{
	PlaneActor::PlaneActor(Game* game) :
		Actor(game)
	{
		SetScale(10.0f);
		MeshComponent* mc = new MeshComponent(this);
		mc->SetMesh(GetGame()->GetRenderer()->GetMesh("src/Assets/3DGraphics/Plane.gpmesh"));
	}
}