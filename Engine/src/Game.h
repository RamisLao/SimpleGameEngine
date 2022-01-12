#pragma once

#include <SDL.h>
#include <vector>
#include <ostream>
#include "Actor.h"


namespace Engine
{
	struct Vector2
	{
		float x;
		float y;
	};

	struct Ball
	{
		Vector2 position;
		Vector2 velocity;
	};

	class Game
	{
	public:
		Game();
		bool Initialize();
		void RunLoop();
		void Shutdown();
		
		void AddActor(Actor* actor);
		void RemoveActor(Actor* actor);
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();

		bool m_IsRunning;
		Uint32 m_TicksCount;

		SDL_Window* m_Window;
		SDL_Renderer* m_Renderer;

		std::vector<Actor*> m_Actors;
		std::vector<Actor*> m_PendingActors;

		// Track if we're updating actors right now
		bool m_UpdatingActors;
	};
}