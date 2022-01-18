
#include "CircleComponent.h"
#include "Actor.h"

namespace Engine
{
	CircleComponent::CircleComponent(Actor* owner) :
		Component(owner),
		m_Radius(0.0f)
	{
	}

	float CircleComponent::GetRadius() const
	{
		return m_Owner->GetScale() * m_Radius;
	}
	const Vector2& CircleComponent::GetCenter() const
	{
		return m_Owner->GetPosition();
	}
	bool CircleComponent::Intersect(const CircleComponent& a, const CircleComponent& b)
	{
		// Calculate distance squared
		Vector2 diff = a.GetCenter() - b.GetCenter();
		float distSq = diff.LengthSq();

		float radiiSq = a.GetRadius() + b.GetRadius();
		radiiSq *= radiiSq;

		return distSq <= radiiSq;
	}
}