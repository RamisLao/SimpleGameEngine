#include "Game.h"
#include <iostream>

std::ostream& operator<<(std::ostream& stream, const Engine::Ball& ball)
{
	stream << "Pos (" << ball.position.x << ", " << ball.position.y << ")";
	stream << " - Vel(" << ball.velocity.x << ", " << ball.velocity.y << ")";

	return stream;
}

namespace Engine
{
	const int thickness = 15;
	const float paddleH = 100.0f;

	Game::Game():
		m_Window(nullptr),
		m_Renderer(nullptr),
		m_IsRunning(true),
		m_TicksCount(0),
		m_LeftPaddleDir(0)
	{
		for (int i = 0; i < 2; i++)
		{
			Ball ball;
			m_Balls.push_back(ball);
		}
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

		for (Ball& ball : m_Balls)
		{
			ball.position.x = 1024 / 2;
			ball.position.y = 768 / 2;

			ball.velocity.x = -100 + rand() % ((300 + 1) + 100);
			ball.velocity.y = -100 + rand() % ((300 + 1) + 100);
		}

		m_LeftPaddlePos.x = thickness / 2 + 10.0f;
		m_LeftPaddlePos.y = 768 / 2;
		m_RightPaddlePos.x = 1024 - thickness / 2 - 10.0f;
		m_RightPaddlePos.y = 768 / 2;

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

		m_LeftPaddleDir = 0;
		if (state[SDL_SCANCODE_W])
		{
			m_LeftPaddleDir -= 1;
		}
		if (state[SDL_SCANCODE_S])
		{
			m_LeftPaddleDir += 1;
		}

		m_RightPaddleDir = 0;
		if (state[SDL_SCANCODE_I])
		{
			m_RightPaddleDir -= 1;
		}
		if (state[SDL_SCANCODE_K])
		{
			m_RightPaddleDir += 1;
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

		if (m_LeftPaddleDir != 0)
		{
			m_LeftPaddlePos.y += m_LeftPaddleDir * 300.0f * deltaTime;

			if (m_LeftPaddlePos.y < (paddleH / 2.0f + thickness))
			{
				m_LeftPaddlePos.y = paddleH / 2.0f + thickness;
			}
			else if (m_LeftPaddlePos.y > 768.0f - paddleH / 2.0f - thickness)
			{
				m_LeftPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}

		if (m_RightPaddleDir != 0)
		{
			m_RightPaddlePos.y += m_RightPaddleDir * 300.0f * deltaTime;

			if (m_RightPaddlePos.y < (paddleH / 2.0f + thickness))
			{
				m_RightPaddlePos.y = paddleH / 2.0f + thickness;
			}
			else if (m_RightPaddlePos.y > 768.0f - paddleH / 2.0f - thickness)
			{
				m_RightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}

		for (Ball& ball : m_Balls)
		{
			ball.position.x += ball.velocity.x * deltaTime;
			ball.position.y += ball.velocity.y * deltaTime;

			// Check collision to top and bottom walls
			if ((ball.position.y <= thickness && ball.velocity.y < 0.0f) ||
				(ball.position.y >= 768.0f - thickness && ball.velocity.y > 0.0f))
			{
				ball.velocity.y *= -1;
			}

			float leftdiff = m_LeftPaddlePos.y - ball.position.y;
			leftdiff = (leftdiff > 0.0f) ? leftdiff : -leftdiff;

			float rightdiff = m_RightPaddlePos.y - ball.position.y;
			rightdiff = (rightdiff > 0.0f) ? rightdiff : -rightdiff;

			if (
				// Check collision with Left paddle
				(leftdiff <= paddleH / 2.0f &&
					ball.position.x <= m_LeftPaddlePos.x + thickness / 2 &&
					ball.position.x >= m_LeftPaddlePos.x - thickness / 2 &&
					ball.velocity.x < 0.0f) ||
				// Check collision with Right paddle
				(rightdiff <= paddleH / 2.0f &&
					ball.position.x >= m_RightPaddlePos.x - thickness / 2 &&
					ball.position.x <= m_RightPaddlePos.x + thickness / 2 &&
					ball.velocity.x > 0.0f)
				)
			{
				ball.velocity.x *= -1.0f;
			}
			else if (ball.position.x <= 0.0f || ball.position.x >= 1024.0f)
			{
				m_IsRunning = false;
			}
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

		SDL_Rect ballRect{
				0,
				0,
				thickness,
				thickness
		};

		for (Ball& ball : m_Balls)
		{
			ballRect.x = static_cast<int>(ball.position.x - thickness / 2);
			ballRect.y = static_cast<int>(ball.position.y - thickness / 2);

			SDL_RenderFillRect(m_Renderer, &ballRect);
		}

		SDL_Rect leftPaddle{
			static_cast<int>(m_LeftPaddlePos.x - thickness / 2),
			static_cast<int>(m_LeftPaddlePos.y - paddleH / 2),
			thickness,
			static_cast<int>(paddleH)
		};
		SDL_RenderFillRect(m_Renderer, &leftPaddle);

		SDL_Rect rightPaddle{
			static_cast<int>(m_RightPaddlePos.x - thickness / 2),
			static_cast<int>(m_RightPaddlePos.y - paddleH / 2),
			thickness,
			static_cast<int>(paddleH)
		};
		SDL_RenderFillRect(m_Renderer, &rightPaddle);

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