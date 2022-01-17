
#include "Actor.h"
#include "Game.h"
#include "Component.h"

namespace Engine
{
	Actor::Actor(Game* game) :
		m_State(EActive),
		m_Position(Vector2::Zero),
		m_Scale(1.0f),
		m_Rotation(0.0f),
		m_Game(game)
	{
		m_Game->AddActor(this);
	}

	Actor::~Actor()
	{
		m_Game->RemoveActor(this);

		// Because ~Component calls RemoveComponent, need a different style loop
		while (!m_Components.empty())
		{
			delete m_Components.back();
		}
	}

	void Actor::Update(float deltaTime)
	{
		if (m_State == EActive)
		{
			UpdateComponents(deltaTime);
			UpdateActor(deltaTime);
		}
	}

	void Actor::UpdateComponents(float deltaTime)
	{
		for (auto comp : m_Components)
		{
			comp->Update(deltaTime);
		}
	}

	void Actor::UpdateActor(float deltaTime)
	{
	}

	void Actor::ProcessInput(const uint8_t* keyState)
	{
		if (m_State == EActive)
		{
			for (auto comp : m_Components)
			{
				comp->ProcessInput(keyState);
			}

			ActorInput(keyState);
		}
	}

	void Actor::ActorInput(const uint8_t* keyState)
	{
	}

	void Actor::AddComponent(Component* component)
	{
		int myOrder = component->GetUpdateOrder();
		auto iter = m_Components.begin();
		for (; iter != m_Components.end(); ++iter)
		{
			if (myOrder < (*iter)->GetUpdateOrder())
			{
				break;
			}
		}

		m_Components.insert(iter, component);
	}

	void Actor::RemoveComponent(Component* component)
	{
		auto iter = std::find(m_Components.begin(), m_Components.end(), component);
		if (iter != m_Components.end())
		{
			m_Components.erase(iter);
		}
	}
}