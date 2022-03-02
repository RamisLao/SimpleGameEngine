#include "AudioSystem.h"
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <SDL_log.h>
#include <vector>
#include "SoundEvent.h"

namespace
{
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// Convert from our coordinates (+x forward, +y right, +z up)
		// to FMOD (+z forward, +x right, +y up)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

namespace Engine
{
	unsigned int AudioSystem::s_NextID = 0;

	AudioSystem::AudioSystem(Game* game)
		: m_Game(game)
		, m_System(nullptr)
		, m_LowLevelSystem(nullptr)
	{
	}

	AudioSystem::~AudioSystem()
	{
	}

	bool AudioSystem::Initialize()
	{
		// Initialize debug logging
		FMOD::Debug_Initialize(
			FMOD_DEBUG_LEVEL_ERROR, // Log only errors
			FMOD_DEBUG_MODE_TTY // Output to stdout
		);

		// Create FMOD studio system object
		FMOD_RESULT result;
		result = FMOD::Studio::System::create(&m_System);
		if (result != FMOD_OK)
		{
			SDL_Log("Failed to create FMOD system: %s", FMOD_ErrorString(result));
			return false;
		}

		// Initialize FMOD studio system
		result = m_System->initialize(
			512, // Max number of concurrent sounds
			FMOD_STUDIO_INIT_NORMAL, // Use default settings
			FMOD_INIT_NORMAL, // FMOD Low-level API Use default settings
			nullptr // In case you want to use extra driver data, Usually null
		);

		if (result != FMOD_OK)
		{
			SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
			return false;
		}

		// Save the low-level system pointer
		m_System->getLowLevelSystem(&m_LowLevelSystem);

		// Load the master banks (strings first)
		// Strings is a special bank that contains the human-readable names of all events and other data
		// Without this bank, the data needs to be accessed through GUIDs (globally unique IDs)
		LoadBank("src/Assets/Audio/Master Bank.strings.bank");
		LoadBank("src/Assets/Audio/Master Bank.bank");

		return true;
	}

	void AudioSystem::Shutdown()
	{
		// Shutdown FMOD system
		if (m_System)
		{
			m_System->release();
		}
	}

	void AudioSystem::Update(float deltaTime)
	{
		// Find any stopped event instances
		std::vector<unsigned int> done;
		for (auto& iter : m_EventInstances)
		{
			FMOD::Studio::EventInstance* e = iter.second;
			// Get the state of this event
			FMOD_STUDIO_PLAYBACK_STATE state;
			e->getPlaybackState(&state);
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
			{
				// Release the event and add id to done
				e->release();
				done.emplace_back(iter.first);
			}
		}

		// Remove done event instances from map
		for (auto id : done)
		{
			m_EventInstances.erase(id);
		}

		// Update FMOD
		m_System->update();
	}

	void AudioSystem::SetListener(const Matrix4& viewMatrix)
	{
		// Invert the view matrix to get the correct vectors
		Matrix4 invView = viewMatrix;
		invView.Invert();
		FMOD_3D_ATTRIBUTES listener;
		// Set position, forward, up
		// In the inverted matrix, the first 3 components of the fourth row are the world space position
		listener.position = VecToFMOD(invView.GetTranslation());
		// In the inverted view, third row is forward
		listener.forward = VecToFMOD(invView.GetZAxis());
		// In the inverted view, second row is up
		listener.up = VecToFMOD(invView.GetYAxis());
		// Set velocity to zero (fix if using Doppler effect)
		listener.velocity = { 0.0f, 0.0f, 0.0f };
		// Send to FMOD
		m_System->setListenerAttributes(0, &listener);
	}

	float AudioSystem::GetBusVolume(const std::string& name) const
	{
		float retVal = 0.0f;
		const auto iter = m_Buses.find(name);
		if (iter != m_Buses.end())
		{
			iter->second->getVolume(&retVal);
		}
		return retVal;
	}

	bool AudioSystem::GetBusPaused(const std::string& name) const
	{
		bool retVal = false;
		const auto iter = m_Buses.find(name);
		if (iter != m_Buses.end())
		{
			iter->second->getPaused(&retVal);
		}
		return retVal;
	}

	void AudioSystem::SetBusVolume(const std::string& name, float volume)
	{
		auto iter = m_Buses.find(name);
		if (iter != m_Buses.end())
		{
			iter->second->setVolume(volume);
		}
	}

	void AudioSystem::SetBusPaused(const std::string& name, bool pause)
	{
		auto iter = m_Buses.find(name);
		if (iter != m_Buses.end())
		{
			iter->second->setPaused(pause);
		}
	}

	FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
	{
		FMOD::Studio::EventInstance* event = nullptr;
		auto iter = m_EventInstances.find(id);
		if (iter != m_EventInstances.end())
		{
			event = iter->second;
		}
		return event;
	}

	void AudioSystem::LoadBank(const std::string& name)
	{
		// Prevent double-loading
		if (m_Banks.find(name) != m_Banks.end())
		{
			return;
		}

		// Try to load bank
		FMOD::Studio::Bank* bank = nullptr;
		FMOD_RESULT result = m_System->loadBankFile(
			name.c_str(), // File name of bank
			FMOD_STUDIO_LOAD_BANK_NORMAL, // Normal loading
			&bank // Save pointer to bank
		);

		const int maxPathLength = 512;
		if (result == FMOD_OK)
		{
			// Add bank to map
			m_Banks.emplace(name, bank);
			// Load all non-streaming sample data
			bank->loadSampleData();
			// Get the number of events in this bank
			int numEvents = 0;
			bank->getEventCount(&numEvents);
			if (numEvents > 0)
			{
				// Get list of event descriptions in this bank
				std::vector<FMOD::Studio::EventDescription*> events(numEvents);
				bank->getEventList(events.data(), numEvents, &numEvents);
				char eventName[maxPathLength];
				for (int i = 0; i < numEvents; i++)
				{
					FMOD::Studio::EventDescription* e = events[i];
					// Get the path of this event (like event:/Explosion2D)
					e->getPath(eventName, maxPathLength, nullptr);
					// Add to event map
					m_Events.emplace(eventName, e);
				}
			}

			// Get the number of buses in this bank
			int numBuses = 0;
			bank->getBusCount(&numBuses);
			if (numBuses > 0)
			{
				// Get list of buses in this bank
				std::vector<FMOD::Studio::Bus*> buses(numBuses);
				bank->getBusList(buses.data(), numBuses, &numBuses);
				char busName[512];
				for (int i = 0; i < numBuses; i++)
				{
					FMOD::Studio::Bus* bus = buses[i];
					// Get the path of this bus (like bus:/SFX)
					bus->getPath(busName, 512, nullptr);
					// Add to buses map
					m_Buses.emplace(busName, bus);
				}
			}
		}
	}

	void AudioSystem::UnloadBank(const std::string& name)
	{
		// Ignore if not loaded
		auto iter = m_Banks.find(name);
		if (iter == m_Banks.end())
		{
			return;
		}

		// First we need to remove all events from this bank
		FMOD::Studio::Bank* bank = iter->second;
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// Get event descriptions for this bank
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			// Get list of events
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[512];
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// Get the path of this event
				e->getPath(eventName, 512, nullptr);
				// Remove this event
				auto eventi = m_Events.find(eventName);
				if (eventi != m_Events.end())
				{
					m_Events.erase(eventi);
				}
			}
		}
		// Get the number of buses in this bank
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// Get list of buses in this bank
			std::vector<FMOD::Studio::Bus*> buses(numBuses);
			bank->getBusList(buses.data(), numBuses, &numBuses);
			char busName[512];
			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* bus = buses[i];
				// Get the path of this bus (like bus:/SFX)
				bus->getPath(busName, 512, nullptr);
				// Remove this bus
				auto busi = m_Buses.find(busName);
				if (busi != m_Buses.end())
				{
					m_Buses.erase(busi);
				}
			}
		}

		// Unload sample data and bank
		bank->unloadSampleData();
		bank->unload();
		// Remove from banks map
		m_Banks.erase(iter);
	}

	void AudioSystem::UnloadAllBanks()
	{
		for (auto& iter : m_Banks)
		{
			// Unload the sample data, then the bank itself
			iter.second->unloadSampleData();
			iter.second->unload();
		}
		m_Banks.clear();
		// No banks means no events
		m_Events.clear();
	}

	SoundEvent AudioSystem::PlayEvent(const std::string& name)
	{
		unsigned int retID = 0;
		auto iter = m_Events.find(name);
		if (iter != m_Events.end())
		{
			// Create instance of event
			FMOD::Studio::EventInstance* event = nullptr;
			iter->second->createInstance(&event);
			if (event)
			{
				// Start the event instance
				event->start();
				// Get the next id, and add to map
				s_NextID++;
				retID = s_NextID;
				m_EventInstances.emplace(retID, event);
			}
		}
		return SoundEvent(this, retID);
	}
}