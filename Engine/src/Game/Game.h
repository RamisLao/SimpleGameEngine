#pragma once

#include <SDL.h>

namespace Engine
{
	struct Vector2
	{
		float x;
		float y;
	};

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

		bool m_IsRunning;
		Uint32 m_TicksCount;

		SDL_Window* m_Window;
		SDL_Renderer* m_Renderer;

		Vector2 m_LeftPaddlePos;
		int m_PaddleDir;
		Vector2 m_BallPos;
		Vector2 m_BallVel;
	};
}