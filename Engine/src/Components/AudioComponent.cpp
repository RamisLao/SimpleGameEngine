
#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"

namespace Engine
{
	AudioComponent::AudioComponent(Actor* owner, int updateOrder)
		:Component(owner, updateOrder)
	{
	}

	AudioComponent::~AudioComponent()
	{
		StopAllEvents();
	}

	void AudioComponent::Update(float deltaTime)
	{
		Component::Update(deltaTime);

		// Remove invalid 2D events
		auto iter = m_Events2D.begin();
		while (iter != m_Events2D.end())
		{
			if (!iter->IsValid())
			{
				iter = m_Events2D.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		// Remove invalid 3D events
		iter = m_Events3D.begin();
		while (iter != m_Events3D.end())
		{
			if (!iter->IsValid())
			{
				iter = m_Events3D.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void AudioComponent::OnUpdateWorldTransform()
	{
		// Update 3D events' world transforms
		Matrix4 world = m_Owner->GetWorldTransform();
		for (auto& event : m_Events3D)
		{
			if (event.IsValid())
			{
				event.Set3DAttributes(world);
			}
		}
	}

	SoundEvent AudioComponent::PlayEvent(const std::string& name)
	{
		SoundEvent e = m_Owner->GetGame()->GetAudioSystem()->PlayEvent(name);
		// Is this 2D or 3D?
		if (e.Is3D())
		{
			m_Events3D.emplace_back(e);
			// Set initial 3D attributes
			e.Set3DAttributes(m_Owner->GetWorldTransform());
		}
		else
		{
			m_Events2D.emplace_back(e);
		}
		return e;
	}

	void AudioComponent::StopAllEvents()
	{
		// Stop all sounds
		for (auto& e : m_Events2D)
		{
			e.Stop();
		}
		for (auto& e : m_Events3D)
		{
			e.Stop();
		}
		// Clear events
		m_Events2D.clear();
		m_Events3D.clear();
	}
}