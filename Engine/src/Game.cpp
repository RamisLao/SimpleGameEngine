#include "Game.h"
#include <iostream>
#include "Actor.h"
// https://www.libsdl.org/projects/SDL_image/
#include "SDL_image.h"
#include "GL/glew.h"
#include "SpriteComponent.h"
#include "Random.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "CameraActor.h"
#include "MeshComponent.h"
#include "Mesh.h"

namespace Engine
{
	const int thickness = 15;
	const float paddleH = 100.0f;

	Game::Game():
		m_Renderer(nullptr),
		m_IsRunning(true),
		m_TicksCount(0)
	{
	}

	bool Game::Initialize()
	{
		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
		{
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
			return false;
		}

		m_Renderer = new Renderer(this);
		if (!m_Renderer->Initialize(1024.f, 768.f))
		{
			SDL_Log("Failed to initialize renderer");
			delete m_Renderer;
			m_Renderer = nullptr;
			return false;
		}

		LoadData();

		m_TicksCount = SDL_GetTicks();

		return true;
	}

	void Game::Shutdown()
	{
		UnloadData();
		if (m_Renderer)
		{
			m_Renderer->Shutdown();
		}
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
			pending->ComputeWorldTransform();
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
		m_Renderer->Draw();
	}

	void Game::LoadData()
	{
		// Create actors
		Actor* a = new Actor(this);
		a->SetPosition(Vector3(200.f, 75.f, 0.f));
		a->SetScale(100.f);
		MeshComponent* mc = new MeshComponent(a);
		mc->SetMesh(m_Renderer->GetMesh("src/Assets/3DGraphics/Cube.gpmesh"));

		m_CameraActor = new CameraActor(this);
	}

	void Game::UnloadData()
	{
		while (!m_Actors.empty())
		{
			delete m_Actors.back();
		}

		if (m_Renderer)
		{
			m_Renderer->UnloadData();
		}
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