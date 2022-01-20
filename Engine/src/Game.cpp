#include "Game.h"
#include <iostream>
#include "Actor.h"
// https://www.libsdl.org/projects/SDL_image/
#include "SDL_image.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Random.h"

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
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
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

		if (IMG_Init(IMG_INIT_PNG) == 0)
		{
			SDL_Log("Unable to initialize SDL_Image: % s", SDL_GetError());
			return false;
		}

		Random::Init();

		LoadData();

		m_TicksCount = SDL_GetTicks();

		return true;
	}

	void Game::Shutdown()
	{
		UnloadData();
		IMG_Quit();
		SDL_DestroyRenderer(m_Renderer);
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

	void Game::AddSprite(SpriteComponent* sprite)
	{
		// Find the insertion point in the sorted vector
		// (The first element with a higher draw order than me)
		int myDrawOrder = sprite->GetDrawOrder();
		auto iter = m_Sprites.begin();
		for (; iter != m_Sprites.end(); ++iter)
		{
			if (myDrawOrder < (*iter)->GetDrawOrder())
			{
				break;
			}
		}

		m_Sprites.insert(iter, sprite);
	}

	void Game::RemoveSprite(SpriteComponent* sprite)
	{
		// We can't swap because it ruins ordering
		auto iter = std::find(m_Sprites.begin(), m_Sprites.end(), sprite);
		m_Sprites.erase(iter);
	}

	SDL_Texture* Game::GetTexture(const std::string& fileName)
	{
		SDL_Texture* tex = nullptr;
		auto iter = m_Textures.find(fileName);
		if (iter != m_Textures.end())
		{
			tex = iter->second;
		}
		else
		{
			SDL_Surface* surf = IMG_Load(fileName.c_str());
			if (!surf)
			{
				SDL_Log("Failed to load texture file %s", SDL_GetError());
				return nullptr;
			}

			tex = SDL_CreateTextureFromSurface(m_Renderer, surf);
			SDL_FreeSurface(surf);
			if (!tex)
			{
				SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
				return nullptr;
			}

			m_Textures.emplace(fileName.c_str(), tex);
		}

		return tex;
	}

	void Game::AddAsteroid(Asteroid* ast)
	{
		m_Asteroids.emplace_back(ast);
	}

	void Game::RemoveAsteroid(Asteroid* ast)
	{
		auto iter = std::find(m_Asteroids.begin(),
			m_Asteroids.end(), ast);
		if (iter != m_Asteroids.end())
		{
			m_Asteroids.erase(iter);
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

		m_UpdatingActors = true;
		for (auto actor : m_Actors)
		{
			actor->ProcessInput(state);
		}
		m_UpdatingActors = false;
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

		// Add any dead actors to a temp vector
		std::vector<Actor*> deadActors;
		for (auto actor : m_Actors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}

		// Delete dead actors (which removes them from m_Actors)
		for (auto actor : deadActors)
		{
			delete actor;
		}
	}

	void Game::GenerateOutput()
	{
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_Renderer);

		for (auto sprite : m_Sprites)
		{
			sprite->Draw(m_Renderer);
		}

		SDL_RenderPresent(m_Renderer);
	}

	void Game::LoadData()
	{
		m_Ship = new Ship(this);
		m_Ship->SetPosition(Vector2(100.0f, 384.0f));
		m_Ship->SetScale(1.5f);

		// Create asteroids
		/*const int numAsteroids = 20;
		for (int i = 0; i < numAsteroids; i++)
		{
			new Asteroid(this);
		}*/
	}

	void Game::UnloadData()
	{
		while (!m_Actors.empty())
		{
			delete m_Actors.back();
		}

		for (auto i : m_Textures)
		{
			SDL_DestroyTexture(i.second);
		}
		m_Textures.clear();
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