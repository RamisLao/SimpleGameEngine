#include "Game.h"

namespace Engine
{
	const int thickness = 15;
	const float paddleH = 100.0f;

	Game::Game():
		m_Window(nullptr),
		m_Renderer(nullptr),
		m_IsRunning(true),
		m_TicksCount(0),
		m_PaddleDir(0)
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

		m_BallPos.x = 1024 / 2;
		m_BallPos.y = 768 / 2;
		m_LeftPaddlePos.x = thickness/2 + 10.0f;
		m_LeftPaddlePos.y = 768 / 2;
		m_BallVel.x = -200.0f;
		m_BallVel.y = 235.0f;

		return true;
	}

	void Game::Shutdown()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
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

		m_PaddleDir = 0;
		if (state[SDL_SCANCODE_W])
		{
			m_PaddleDir -= 1;
		}
		if (state[SDL_SCANCODE_S])
		{
			m_PaddleDir += 1;
		}
	}

	void Game::UpdateGame()
	{
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), m_TicksCount + 16));

		float deltaTime = (SDL_GetTicks() - m_TicksCount) / 1000.0f;
		m_TicksCount = SDL_GetTicks();

		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}

		if (m_PaddleDir != 0)
		{
			m_LeftPaddlePos.y += m_PaddleDir * 300.0f * deltaTime;

			if (m_LeftPaddlePos.y < (paddleH / 2.0f + thickness))
			{
				m_LeftPaddlePos.y = paddleH / 2.0f + thickness;
			}
			else if (m_LeftPaddlePos.y > 768.0f - paddleH / 2.0f - thickness)
			{
				m_LeftPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}

		m_BallPos.x += m_BallVel.x * deltaTime;
		m_BallPos.y += m_BallVel.y * deltaTime;

		// Check collision to top and bottom walls
		if ((m_BallPos.y <= thickness && m_BallVel.y < 0.0f) ||
			(m_BallPos.y >= 768.0f - thickness && m_BallVel.y > 0.0f))
		{
			m_BallVel.y *= -1;
		}

		float diff = m_LeftPaddlePos.y - m_BallPos.y;
		diff = (diff > 0.0f) ? diff : -diff;

		if (
		// Check collision with paddle
			(diff <= paddleH / 2.0f &&
			m_BallPos.x <= m_LeftPaddlePos.x + thickness / 2 &&
			m_BallPos.x >= m_LeftPaddlePos.x - thickness / 2 &&
			m_BallVel.x < 0.0f) ||
		// Check collision with right wall
			((m_BallPos.x >= (1024 - thickness) && m_BallPos.x > 0.0f))
			)
		{
			m_BallVel.x *= -1.0f;
		}
		else if (m_BallPos.x <= 0.0f)
		{
			m_IsRunning = false;
		}
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

		// Draw top wall
		SDL_Rect wall {
			0,
			0,
			1024,
			thickness
		};
		SDL_RenderFillRect(m_Renderer, &wall);

		// Draw bottom wall
		wall.y = 768 - thickness;
		SDL_RenderFillRect(m_Renderer, &wall);

		// Draw right wall
		wall.x = 1024 - thickness;
		wall.y = 0;
		wall.w = thickness;
		wall.h = 768;
		SDL_RenderFillRect(m_Renderer, &wall);

		SDL_Rect ball{
			static_cast<int>(m_BallPos.x - thickness/2),
			static_cast<int>(m_BallPos.y - thickness/2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(m_Renderer, &ball);

		SDL_Rect leftPaddle{
			static_cast<int>(m_LeftPaddlePos.x - thickness / 2),
			static_cast<int>(m_LeftPaddlePos.y - paddleH / 2),
			thickness,
			static_cast<int>(paddleH)
		};
		SDL_RenderFillRect(m_Renderer, &leftPaddle);

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