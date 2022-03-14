#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"

namespace Engine
{
	BoxComponent::BoxComponent(Actor* owner, int updateOrder)
		:Component(owner, updateOrder)
		, m_ObjectBox(Vector3::Zero, Vector3::Zero)
		, m_WorldBox(Vector3::Zero, Vector3::Zero)
		, m_ShouldRotate(true)
	{
		m_Owner->GetGame()->GetPhysWorld()->AddBox(this);
	}

	BoxComponent::~BoxComponent()
	{
		m_Owner->GetGame()->GetPhysWorld()->RemoveBox(this);
	}

	void BoxComponent::OnUpdateWorldTransform()
	{
		// Reset to object space box
		m_WorldBox = m_ObjectBox;
		// Scale
		m_WorldBox.m_Min *= m_Owner->GetScale();
		m_WorldBox.m_Max *= m_Owner->GetScale();
		// Rotate (if we want to)
		if (m_ShouldRotate)
		{
			m_WorldBox.Rotate(m_Owner->GetRotation());
		}
		// Translate
		m_WorldBox.m_Min += m_Owner->GetPosition();
		m_WorldBox.m_Max += m_Owner->GetPosition();
	}
}