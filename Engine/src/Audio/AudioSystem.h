#pragma once
#include <unordered_map>
#include <string>
#include "Math.h"
#include "SoundEvent.h"

// Forward declarations to avoid including FMOD header
namespace FMOD
{
	class System;
	namespace Studio
	{
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};

namespace Engine
{
	class AudioSystem
	{
	public:
		AudioSystem(class Game* game);
		~AudioSystem();

		bool Initialize();
		void Shutdown();

		// Load/unload banks
		void LoadBank(const std::string& name);
		void UnloadBank(const std::string& name);
		void UnloadAllBanks();

		SoundEvent PlayEvent(const std::string& name);

		void Update(float deltaTime);

		// For positional audio
		void SetListener(const Matrix4& viewMatrix);
		// Control buses
		float GetBusVolume(const std::string& name) const;
		bool GetBusPaused(const std::string& name) const;
		void SetBusVolume(const std::string& name, float volume);
		void SetBusPaused(const std::string& name, bool pause);
	protected:
		friend class SoundEvent;
		FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);
	private:
		// Tracks the next ID to use for event instances
		static unsigned int s_NextID;

		class Game* m_Game;

		// Map of loaded banks
		// The string is the filename of the bank
		std::unordered_map<std::string, FMOD::Studio::Bank*> m_Banks;
		// Map of event name to EventDescription
		// EventDescriptions contain information about an event (associated sample data, volume settings, parameters, etc
		// It is like a "type" of event, while an EventInstance is an instance of that type
		// The string is the name assigned by FMOD to that event (event:/Explosion3D)
		std::unordered_map<std::string, FMOD::Studio::EventDescription*> m_Events;
		// Map of event id to EventInstance
		std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> m_EventInstances;
		// Map of buses
		std::unordered_map<std::string, FMOD::Studio::Bus*> m_Buses;

		// FMOD studio system
		FMOD::Studio::System* m_System;
		// FMOD Low-level system (in case needed)
		FMOD::System* m_LowLevelSystem;
	};
}