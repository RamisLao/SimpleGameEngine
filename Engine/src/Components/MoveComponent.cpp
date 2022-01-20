
#include "MoveComponent.h"
#include "Actor.h"
#include "CustomMath.h"

namespace Engine
{
	MoveComponent::MoveComponent(Actor* owner, int updateOrder) :
		Component(owner, updateOrder),
		m_AngularSpeed(0.0f),
		m_ForwardSpeed(0.0f),
		m_SumOfForces(Vector2(1.0f, 1.0f)),
		m_Mass(1),
		m_MaxAcceleration(200)
	{
	}

	void MoveComponent::Update(float deltaTime)
	{
		if (!CustomMath::NearZero(m_AngularSpeed))
		{
			float rot = m_Owner->GetRotation();
			rot += m_AngularSpeed * deltaTime;
			m_Owner->SetRotation(rot);
		}

		if (!CustomMath::NearZero(m_SumOfForces.Length()))
		{
			Vector2 pos = m_Owner->GetPosition();

			/*Vector2 acceleration = m_SumOfForces / m_Mass;
			m_SumOfForces = Vector2::Zero;

			if (acceleration.Length() >= m_MaxAcceleration)
			{
				acceleration.Normalize();
				acceleration *= m_MaxAcceleration;
			}

			m_Velocity += acceleration * deltaTime;
			pos += m_Velocity * deltaTime;*/
			pos += m_Owner->GetForward() * m_ForwardSpeed * deltaTime;

			// (Screen wrapping code for asteroids)
			if (pos.x < 0.0f) { pos.x = 1022.0f; }
			else if (pos.x > 1024.0f) { pos.x = 2.0f; }

			if (pos.y < 0.0f) { pos.y = 766.0f; }
			else if (pos.y > 768.0f) { pos.y = 2.0f; }

			m_Owner->SetPosition(pos);
		}
	}

	void MoveComponent::AddForce(Vector2& force)
	{
		m_SumOfForces += force;
	}
}