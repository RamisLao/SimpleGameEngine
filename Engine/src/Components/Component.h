#pragma once
#include <cstdint>

namespace Engine
{
	class Component
	{
	public:
		// The lower the update order, the earlier the component updates
		Component(class Actor* owner, int updateOrder = 100);
		virtual ~Component();
		virtual void Update(float deltaTime);
		virtual void ProcessInput(const struct InputState& state) {}
		virtual void OnUpdateWorldTransform() {}

		int GetUpdateOrder() const { return m_UpdateOrder; }

	protected:
		class Actor* m_Owner;
		int m_UpdateOrder;
	};
}