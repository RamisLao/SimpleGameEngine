#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <unordered_map>
#include <vector>
#include <ostream>
#include "CustomMath.h"
#include "SoundEvent.h"

namespace Engine
{
	class Game
	{
	public:
		Game();
		bool Initialize();
		void RunLoop();
		
		void Shutdown();
		
		void AddActor(class Actor* actor);
		void RemoveActor(class Actor* actor);

		class Renderer* GetRenderer() { return m_Renderer; }
		class AudioSystem* GetAudioSystem() { return m_AudioSystem; }
	private:
		void ProcessInput();
		void HandleKeyPress(int key);
		void UpdateGame();
		void GenerateOutput();
		void LoadData();
		void UnloadData();

		bool m_IsRunning;
		Uint32 m_TicksCount;

		class Renderer* m_Renderer;
		class AudioSystem* m_AudioSystem;
		class InputSystem* m_InputSystem;

		std::vector<class Actor*> m_Actors;
		std::vector<class Actor*> m_PendingActors;
		// Track if we're updating actors right now
		bool m_UpdatingActors;

		// Game-specific code
		class FPSActor* m_FPSActor;
		class FollowActor* m_FollowActor;
		SoundEvent m_MusicEvent;
		SoundEvent m_ReverbSnap;
	};
}