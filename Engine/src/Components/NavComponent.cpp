
#include "NavComponent.h"
#include "Actor.h"
#include "Tile.h"

namespace Engine
{
	NavComponent::NavComponent(Actor* owner, int updateOrder) :
		MoveComponent(owner, updateOrder),
		m_NextNode(nullptr)
	{
	}

	void NavComponent::Update(float deltaTime)
	{
		if (m_NextNode)
		{
			Vector2 diff = m_Owner->GetPosition() - m_NextNode->GetPosition();
			if (CustomMath::NearZero(diff.Length(), 2.0f))
			{
				m_NextNode = m_NextNode->GetParent();
				TurnTo(m_NextNode->GetPosition());
			}
		}

		MoveComponent::Update(deltaTime);
	}

	void NavComponent::StartPath(const Tile* start)
	{
		m_NextNode = start->GetParent();
		TurnTo(m_NextNode->GetPosition());
	}

	void NavComponent::TurnTo(const Vector2& pos)
	{
		Vector2 dir = pos - m_Owner->GetPosition();
		// New angle is just atan2 of this dir vector
		// (Negate y because +y is down on screen
		float angle = CustomMath::Atan2(-dir.y, dir.x);
		m_Owner->SetRotation(angle);
	}
}