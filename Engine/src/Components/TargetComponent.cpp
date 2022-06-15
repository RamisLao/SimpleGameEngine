#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "TargetComponent.h"
#include "Math.h"
#include "Game.h"
#include "HUD.h"

namespace Engine
{
	TargetComponent::TargetComponent(Actor* owner)
		:Component(owner)
	{
		m_Owner->GetGame()->GetHUD()->AddTargetComponent(this);
	}

	TargetComponent::~TargetComponent()
	{
		m_Owner->GetGame()->GetHUD()->RemoveTargetComponent(this);
	}
}