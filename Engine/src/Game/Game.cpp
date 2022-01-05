#include "Game.h"

namespace Engine
{
	Game::Game():
		m_IsRunning(true)
	{
	}

	bool Game::Initialize()
	{
		int sdlResult = SDL_Init(SDL_INIT_VIDEO);

		if (sdlResult != 0)
		{
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
			return false;
		}

		m_Window = SDL_CreateWindow(
			"Simple Game Engine",
			100,
			100,
			1024,
			768,
			0
		);

		if (!m_Window)
		{
			SDL_Log("Failed to create window: % s", SDL_GetError());
			return false;
		}

		return true;
	}

	void Game::Shutdown()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	void Game::ProcessInput()
	{
	}

	void Game::UpdateGame()
	{
	}

	void Game::GenerateOutput()
	{
	}

	void Game::RunLoop()
	{
		while (m_IsRunning)
		{
			ProcessInput();
			UpdateGame();
			GenerateOutput();
		}
	}
}