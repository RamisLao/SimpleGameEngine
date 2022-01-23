
#include "AIComponent.h"
#include "AIState.h"
#include "SDL_log.h"

namespace Engine
{
	AIComponent::AIComponent(class Actor* owner) :
		Component(owner),
		m_CurrentState(nullptr)
	{
	}

	void AIComponent::Update(float deltaTime)
	{
		if (m_CurrentState)
		{
			m_CurrentState->Update(deltaTime);
		}
	}

	void AIComponent::ChangeState(const std::string& name)
	{
		if (m_CurrentState)
		{
			m_CurrentState->OnExit();
		}

		auto iter = m_StateMap.find(name);
		if (iter != m_StateMap.end())
		{
			m_CurrentState = iter->second;
			m_CurrentState->OnEnter();
		}
		else
		{
			SDL_Log("Could not find AIState %s in state map", name.c_str());
			m_CurrentState = nullptr;
		}
	}

	void AIComponent::RegisterState(AIState* state)
	{
		m_StateMap.emplace(state->GetName(), state);
	}
}