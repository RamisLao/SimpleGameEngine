#pragma once

#include <SDL.h>
#include <vector>
#include <ostream>


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
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();

		bool m_IsRunning;
		Uint32 m_TicksCount;

		SDL_Window* m_Window;
		SDL_Renderer* m_Renderer;

		Vector2 m_LeftPaddlePos;
		int m_LeftPaddleDir;
		Vector2 m_RightPaddlePos;
		int m_RightPaddleDir;

		std::vector<Ball> m_Balls;
	};
}