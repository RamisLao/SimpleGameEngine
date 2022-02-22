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
#include "Asteroids\Ship.h"
#include "Texture.h"
#include "Renderer.h"
#include "CameraActor.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "PlaneActor.h"
#include "AudioSystem.h"

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

		// Create the audio system
		m_AudioSystem = new AudioSystem(this);
		if (!m_AudioSystem->Initialize())
		{
			SDL_Log("Failed to initialize audio system");
			m_AudioSystem->Shutdown();
			delete m_AudioSystem;
			m_AudioSystem = nullptr;
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
		if (m_AudioSystem)
		{
			m_AudioSystem->Shutdown();
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

		// Update audio system
		m_AudioSystem->Update(deltaTime);
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
		Quaternion q(Vector3::UnitY, -CustomMath::PiOver2);
		q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, CustomMath::Pi + CustomMath::Pi / 4.0f));
		a->SetRotation(q);
		MeshComponent* mc = new MeshComponent(a);
		mc->SetMesh(m_Renderer->GetMesh("src/Assets/3DGraphics/Cube.gpmesh"));

		a = new Actor(this);
		a->SetPosition(Vector3(200.0f, -75.0f, 0.0f));
		a->SetScale(3.0f);
		mc = new MeshComponent(a);
		mc->SetMesh(m_Renderer->GetMesh("src/Assets/3DGraphics/Sphere.gpmesh"));

		// Setup floor
		const float start = -1250.0f;
		const float size = 250.0f;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				a = new PlaneActor(this);
				a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
			}
		}

		//// Left/right walls
		q = Quaternion(Vector3::UnitX, CustomMath::PiOver2);
		for (int i = 0; i < 10; i++)
		{
			a = new PlaneActor(this);
			a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
			a->SetRotation(q);

			a = new PlaneActor(this);
			a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
			a->SetRotation(q);
		}

		q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, CustomMath::PiOver2));

		//// Forward/back walls
		for (int i = 0; i < 10; i++)
		{
			a = new PlaneActor(this);
			a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
			a->SetRotation(q);

			a = new PlaneActor(this);
			a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
			a->SetRotation(q);
		}

		m_Renderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
		DirectionalLight& dir = m_Renderer->GetDirectionalLight();
		dir.m_Direction = Vector3(0.0f, -0.707f, -0.707f);
		dir.m_DiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
		dir.m_SpecColor = Vector3(0.8f, 0.8f, 0.8f);

		m_CameraActor = new CameraActor(this);

		// UI elements
		a = new Actor(this);
		a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
		SpriteComponent* sc = new SpriteComponent(a);
		sc->SetTexture(m_Renderer->GetTexture("src/Assets/3DGraphics/HealthBar.png"));

		a = new Actor(this);
		a->SetPosition(Vector3(375.0f, -275.0f, 0.0f));
		a->SetScale(0.75f);
		sc = new SpriteComponent(a);
		sc->SetTexture(m_Renderer->GetTexture("src/Assets/3DGraphics/Radar.png"));

		m_MusicEvent = m_AudioSystem->PlayEvent("event:/Music");
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