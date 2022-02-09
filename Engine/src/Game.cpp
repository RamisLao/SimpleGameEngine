#include "Game.h"
#include <iostream>
#include "Actor.h"
// https://www.libsdl.org/projects/SDL_image/
#include "SDL_image.h"
#include "GL/glew.h"
#include "SpriteComponent.h"
#include "Random.h"
#include "GameObjects/Asteroids/Asteroid.h"
#include "GameObjects/Asteroids/Ship.h"
#include "GameObjects/TowerDefense/Enemy.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

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
		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
		{
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
			return false;
		}

		// Set OpenGL attributes
		// Use the core OpenGL profile
		// Available profiles: Core, Compatibility (deprecated features available), ES (mobile)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
							SDL_GL_CONTEXT_PROFILE_CORE);
		// Specify version 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		// Request a color buffer with 8-bits per RGBA channel
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		// Enable double buffering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		// Run on GPU
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		m_Window = SDL_CreateWindow(
			"Simple Game Engine",
			100,
			100,
			1024,
			768,
			SDL_WINDOW_OPENGL
		);

		if (!m_Window)
		{
			SDL_Log("Failed to create window: % s", SDL_GetError());
			return false;
		}

		m_Context = SDL_GL_CreateContext(m_Window);

		// Prevents initialization errors
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			SDL_Log("Failed to initialize GLEW");
			return false;
		}

		// Clear benign error on some platforms
		glGetError();

		if (!LoadShaders())
		{
			SDL_Log("Failed to load shaders.");
			return false;
		}

		// Create quad for drawing sprites
		CreateSpriteVerts();

		LoadData();

		m_TicksCount = SDL_GetTicks();

		return true;
	}

	void Game::Shutdown()
	{
		UnloadData();
		delete m_SpriteVerts;
		m_SpriteShader->Unload();
		delete m_SpriteShader;
		SDL_GL_DeleteContext(m_Context);
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

	Texture* Game::GetTexture(const std::string& fileName)
	{
		Texture* tex = nullptr;
		auto iter = m_Textures.find(fileName);
		if (iter != m_Textures.end())
		{
			tex = iter->second;
		}
		else
		{
			tex = new Texture();
			if (tex->Load(fileName))
			{
				m_Textures.emplace(fileName, tex);
			}
			else
			{
				delete tex;
				tex = nullptr;
			}
		}

		return tex;
	}

	Enemy* Game::GetNearestEnemy(const Vector2& pos)
	{
		Enemy* best = nullptr;

		if (m_Enemies.size() > 0)
		{
			best = m_Enemies[0];
			// Save the distance squared of first enemy, and test if others are closer
			float bestDistSq = (pos - m_Enemies[0]->GetPosition()).LengthSq();
			for (size_t i = 1; i < m_Enemies.size(); i++)
			{
				float newDistSq = (pos - m_Enemies[i]->GetPosition()).LengthSq();
				if (newDistSq < bestDistSq)
				{
					bestDistSq = newDistSq;
					best = m_Enemies[i];
				}
			}
		}

		return best;
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
		glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_SpriteShader->SetActive();
		m_SpriteVerts->SetActive();

		for (auto sprite : m_Sprites)
		{
			sprite->Draw(m_SpriteShader);
		}

		SDL_GL_SwapWindow(m_Window);
	}

	bool Game::LoadShaders()
	{
		m_SpriteShader = new Shader();
		if (!m_SpriteShader->Load("src/Shaders/Sprite.vert", "src/Shaders/Sprite.frag"))
		{
			return false;
		}

		m_SpriteShader->SetActive();
		// Set the view-projection matrix
		Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
		m_SpriteShader->SetMatrixUniform("uViewProj", viewProj);

		return true;
	}

	void Game::CreateSpriteVerts()
	{
		float vertices[] = {
			-0.5f,  0.5f, 0.f, 0.f, 0.f, // top left
			 0.5f,  0.5f, 0.f, 1.f, 0.f, // top right	
			 0.5f, -0.5f, 0.f, 1.f, 1.f, // bottom right
			-0.5f, -0.5f, 0.f, 0.f, 1.f  // bottom left
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_SpriteVerts = new VertexArray(vertices, 4, indices, 6);
	}

	void Game::LoadData()
	{
		m_Ship = new Ship(this);
		m_Ship->SetRotation(CustomMath::PiOver2);

		//// Create asteroids
		//const int numAsteroids = 20;
		//for (int i = 0; i < numAsteroids; i++)
		//{
		//	new Asteroid(this);
		//}
	}

	void Game::UnloadData()
	{
		while (!m_Actors.empty())
		{
			delete m_Actors.back();
		}

		for (auto i : m_Textures)
		{
			i.second->Unload();
			delete i.second;
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
}