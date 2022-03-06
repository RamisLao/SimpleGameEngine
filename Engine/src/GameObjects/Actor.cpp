
#include "Actor.h"
#include "Game.h"
#include "Component.h"

namespace Engine
{
	Actor::Actor(Game* game) :
		m_State(EActive),
		m_Position(Vector3::Zero),
		m_Scale(1.0f),
		m_Rotation(Quaternion::Identity),
		m_Game(game),
		m_RecomputeWorldTransform(true)
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
		if (m_State == EActive || m_State == EInvisible)
		{
			ComputeWorldTransform();

			UpdateComponents(deltaTime);
			UpdateActor(deltaTime);

			ComputeWorldTransform();
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

	void Actor::ProcessInput(const struct InputState& state)
	{
		if (m_State == EActive)
		{
			// First process input for components
			for (auto comp : m_Components)
			{
				comp->ProcessInput(state);
			}

			ActorInput(state);
		}
	}

	void Actor::ActorInput(const struct InputState& state)
	{
	}

	void Actor::ComputeWorldTransform()
	{
		if (m_RecomputeWorldTransform)
		{
			m_RecomputeWorldTransform = false;
			m_WorldTransform = Matrix4::CreateScale(m_Scale);
			m_WorldTransform *= Matrix4::CreateFromQuaternion(m_Rotation);
			m_WorldTransform *= Matrix4::CreateTranslation(m_Position);

			for (auto comp : m_Components)
			{
				comp->OnUpdateWorldTransform();
			}
		}

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