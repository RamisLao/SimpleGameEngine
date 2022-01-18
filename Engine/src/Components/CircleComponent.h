#pragma once
#include "Component.h"
#include "CustomMath.h"

namespace Engine
{
	class CircleComponent : public Component
	{
	public:
		CircleComponent(class Actor* owner);
		
		void SetRadius(float radius) { m_Radius = radius; }
		float GetRadius() const;

		const Vector2& GetCenter() const;

		static bool Intersect(const CircleComponent& a, const CircleComponent& b);
	private:
		float m_Radius;
	};
}