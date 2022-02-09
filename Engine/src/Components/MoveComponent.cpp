
#include "MoveComponent.h"
#include "Actor.h"
#include "CustomMath.h"
#include <iostream>
#include "SDL_log.h"

namespace Engine
{
	MoveComponent::MoveComponent(Actor* owner, int updateOrder) :
		Component(owner, updateOrder),
		m_AngularSpeed(0.0f),
		m_ForwardSpeed(0.0f),
		m_Static(false),
		m_SumOfForces(Vector2::Zero),
		m_Mass(0.0f),
		m_Drag(0.981f),
		m_MaxVelocity(0.0f),
		m_ScreenWrap(false)
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

		if (m_Static) return;

		Vector2 pos = m_Owner->GetPosition();

		if (m_ForwardSpeed > 0)
		{
			Vector2 forwardSpeed = m_Owner->GetForward() * m_ForwardSpeed * deltaTime;
			AddForce(forwardSpeed);
		}

		Vector2 acceleration = m_SumOfForces / m_Mass;
		m_SumOfForces = Vector2::Zero;

		m_Velocity += acceleration * deltaTime;
		m_Velocity *= m_Drag;
		if (m_Velocity.Length() >= m_MaxVelocity)
		{
			m_Velocity.Normalize();
			m_Velocity *= m_MaxVelocity;
		}

		pos += m_Velocity * deltaTime;

		if (m_ScreenWrap)
		{
			// (Screen wrapping code for asteroids)
			if (pos.x < -512.0f) { pos.x = 510.f; }
			else if (pos.x > 512.0f) { pos.x = -510.f; }

			if (pos.y < -384.0f) { pos.y = 382.0f; }
			else if (pos.y > 384.0f) { pos.y = -382.0f; }
		}

		m_Owner->SetPosition(pos);
	}

	void MoveComponent::AddForce(Vector2& force)
	{
		m_SumOfForces += force;
	}
}