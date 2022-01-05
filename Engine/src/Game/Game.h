#pragma once

#include <SDL.h>

namespace Engine
{
	class Game
	{
	public:
		Game();
		bool Initialize();
		void RunLoop();
		void Shutdown();
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();

		SDL_Window* m_Window;
		bool m_IsRunning;
	};
}