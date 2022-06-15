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
		class PhysWorld* GetPhysWorld() { return m_PhysWorld; }
		class HUD* GetHUD() { return m_HUD; }
		class FPSActor* GetPlayer() { return m_FPSActor; }

		enum GameState
		{
			EGameplay,
			EPaused,
			EQuit
		};

		GameState GetState() const { return m_GameState; }
		void SetState(GameState state) { m_GameState = state; }

		// Manage UI stack
		const std::vector<class UIScreen*>& GetUIStack() { return m_UIStack; }
		void PushUI(class UIScreen* screen);

		class Font* GetFont(const std::string& fileName);
		void LoadText(const std::string& fileName);
		const std::string& GetText(const std::string& key);

		// Game-specific
		void AddPlane(class PlaneActor* plane);
		void RemovePlane(class PlaneActor* plane);
		std::vector<class PlaneActor*>& GetPlanes() { return m_Planes; }
	private:
		void ProcessInput();
		void HandleKeyPress(int key);
		void UpdateGame();
		void GenerateOutput();
		void LoadData();
		void UnloadData();

		GameState m_GameState;
		bool m_IsRunning;
		Uint32 m_TicksCount;

		class Renderer* m_Renderer;
		class AudioSystem* m_AudioSystem;
		class InputSystem* m_InputSystem;
		class PhysWorld* m_PhysWorld;
		class HUD* m_HUD;

		std::vector<class Actor*> m_Actors;
		std::vector<class Actor*> m_PendingActors;
		// Track if we're updating actors right now
		bool m_UpdatingActors;

		std::unordered_map<std::string, class Font*> m_Fonts;
		std::vector<class UIScreen*> m_UIStack;
		// Map for text localization
		std::unordered_map<std::string, std::string> m_Text;

		// Game-specific code
		class FPSActor* m_FPSActor;
		class FollowActor* m_FollowActor;
		SoundEvent m_MusicEvent;
		SoundEvent m_ReverbSnap;
		class SpriteComponent* m_Crosshair;
		std::vector<class PlaneActor*> m_Planes;
	};
}