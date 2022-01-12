#include "Game.h"
#include <iostream>

namespace Engine
{
	const int thickness = 15;
	const float paddleH = 100.0f;

	Game::Game():
		m_Window(nullptr),
		m_Renderer(nullptr),
		m_IsRunning(true),
		m_TicksCount(0)
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

		m_Renderer = SDL_CreateRenderer(
			m_Window,
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);

		return true;
	}

	void Game::Shutdown()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	void Game::AddActor(Actor* actor)
	{
		if (m_UpdatingActors)
		{
			m_PendingActors.emplace_back(actor);
		}
		else
		{
			m_Actors.emplace_back(actor);
		}
	}

	void Game::RemoveActor(Actor* actor)
	{
		// Is it in pending actors?
		auto iter = std::find(m_PendingActors.begin(), m_PendingActors.end(), actor);
		if (iter != m_PendingActors.end())
		{
			std::iter_swap(iter, m_PendingActors.end() - 1);
			m_PendingActors.pop_back();
		}

		// Is it in actors?
		iter = std::find(m_Actors.begin(), m_Actors.end(), actor);
		if (iter != m_Actors.end())
		{
			std::iter_swap(iter, m_Actors.end() - 1);
			m_Actors.pop_back();
		}
	}

	void Game::ProcessInput()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				m_IsRunning = false;
				break;
			}
		}

		const Uint8* state = SDL_GetKeyboardState(NULL);

		if (state[SDL_SCANCODE_ESCAPE])
		{
			m_IsRunning = false;
		}
	}

	void Game::UpdateGame()
	{
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), m_TicksCount + 16));

		float deltaTime = (SDL_GetTicks() - m_TicksCount) / 1000.0f;

		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}
		m_TicksCount = SDL_GetTicks();

		// Update all actors
		m_UpdatingActors = true;
		for (auto actor : m_Actors)
		{
			actor->Update(deltaTime);
		}

		// Move any pending actors to m_Actors
		for (auto pending : m_PendingActors)
		{
			m_Actors.emplace_back(pending);
		}
		m_PendingActors.clear();
	}

	void Game::GenerateOutput()
	{
		SDL_SetRenderDrawColor(
			m_Renderer,
			0,
			0,
			255,
			255
		);

		SDL_RenderClear(m_Renderer);

		SDL_SetRenderDrawColor(
			m_Renderer,
			255,
			255,
			255,
			255
		);

		SDL_RenderPresent(m_Renderer);
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