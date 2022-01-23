#pragma once
#include "Component.h"
#include <string>
#include <unordered_map>

namespace Engine
{
	class AIComponent : public Component
	{
	public:
		AIComponent(class Actor* owner);

		void Update(float deltaTime) override;
		void ChangeState(const std::string& name);

		void RegisterState(class AIState* state);
	private:
		// Maps name of State to AIState instance
		std::unordered_map<std::string, class AIState*> m_StateMap;
		class AIState* m_CurrentState;
	};
}