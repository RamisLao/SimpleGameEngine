#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>
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
#include "MeshComponent.h"
#include "Mesh.h"
#include "PlaneActor.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "InputSystem.h"
#include "FPSActor.h"
#include "FollowActor.h"
#include "CameraActor.h"
#include "PhysWorld.h"
#include "TargetActor.h"
#include "SDL_ttf.h"
#include "Font.h"
#include <rapidjson\document.h>
#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"

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
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
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

		m_InputSystem = new InputSystem();
		if (!m_InputSystem->Initialize())
		{
			SDL_Log("Failed to initialize input system");
			return false;
		}
		m_InputSystem->SetRelativeMouseMode(true);

		SDL_GameControllerAddMappingsFromFile("src/Assets/gamecontrollerdb.txt");

		m_PhysWorld = new PhysWorld(this);

		// Initialize SDL_ttf
		if (TTF_Init() != 0)
		{
			SDL_Log("Failed to initialize SDL_ttf");
			return false;
		}

		LoadData();
		Random::Init();

		std::cout << __cplusplus << std::endl;

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

	void Game::PushUI(UIScreen* screen)
	{
		m_UIStack.emplace_back(screen);
	}

	Font* Game::GetFont(const std::string& fileName)
	{
		auto iter = m_Fonts.find(fileName);
		if (iter != m_Fonts.end())
		{
			return iter->second;
		}
		else
		{
			Font* font = new Font(this);
			if (font->Load(fileName))
			{
				m_Fonts.emplace(fileName, font);
			}
			else
			{
				font->Unload();
				delete font;
				font = nullptr;
			}
			return font;
		}
	}

	void Game::LoadText(const std::string& fileName)
	{
		// Clear the existing map, if already loaded
		m_Text.clear();
		// Try to open the file
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			SDL_Log("Text file %s not found", fileName.c_str());
			return;
		}
		// Read the entire file to a string stream
		std::stringstream fileStream;
		fileStream << file.rdbuf();
		std::string contents = fileStream.str();
		// Open this file in rapidJSON
		rapidjson::StringStream jsonStr(contents.c_str());
		rapidjson::Document doc;
		doc.ParseStream(jsonStr);
		if (!doc.IsObject())
		{
			SDL_Log("Text file %s is not valid JSON", fileName.c_str());
			return;
		}
		// Parse the text map
		const rapidjson::Value& actions = doc["TextMap"];
		for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
			itr != actions.MemberEnd(); ++itr)
		{
			if (itr->name.IsString() && itr->value.IsString())
			{
				m_Text.emplace(itr->name.GetString(),
					itr->value.GetString());
			}
		}
	}

	const std::string& Game::GetText(const std::string& key)
	{
		static std::string errorMsg("**KEY NOT FOUND**");
		// Find this text in the map, if it exists
		auto iter = m_Text.find(key);
		if (iter != m_Text.end())
		{
			return iter->second;
		}
		else
		{
			return errorMsg;
		}
	}

	void Game::AddPlane(PlaneActor* plane)
	{
		m_Planes.emplace_back(plane);
	}

	void Game::RemovePlane(PlaneActor* plane)
	{
		auto iter = std::find(m_Planes.begin(), m_Planes.end(), plane);
		m_Planes.erase(iter);
	}

	void Game::ProcessInput()
	{
		m_InputSystem->PrepareForUpdate();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				m_IsRunning = false;
				break;
			// This fires when a key's initially pressed
			case SDL_KEYDOWN:
				if (m_GameState == EGameplay)
				{
					HandleKeyPress(event.key.keysym.sym);
				}
				else if (!m_UIStack.empty())
				{
					m_UIStack.back()->
						HandleKeyPress(event.key.keysym.sym);
				}
				break;
			case SDL_MOUSEWHEEL:
				m_InputSystem->ProcessEvent(event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (m_GameState == EGameplay)
				{
					HandleKeyPress(event.button.button);
				}
				else if (!m_UIStack.empty())
				{
					m_UIStack.back()->
						HandleKeyPress(event.button.button);
				}
				break;
			default:
				break;
			}
		}

		m_InputSystem->Update();
		const InputState& state = m_InputSystem->GetState();

		if (m_GameState == EGameplay)
		{
			m_UpdatingActors = true;
			for (auto actor : m_Actors)
			{
				if (actor->GetState() == Actor::EActive)
				{
					actor->ProcessInput(state);
				}
			}
			m_UpdatingActors = false;
		}
		else if (!m_UIStack.empty())
		{
			const Uint8* state = SDL_GetKeyboardState(NULL);
			m_UIStack.back()->ProcessInput(state);
		}
	}

	void Game::HandleKeyPress(int key)
	{
		switch (key)
		{
		case SDLK_ESCAPE:
			// Create pause menu
			new PauseMenu(this);
			break;
		case '-':
		{
			// Reduce master volume
			float volume = m_AudioSystem->GetBusVolume("bus:/");
			volume = CustomMath::Max(0.0f, volume - 0.1f);
			m_AudioSystem->SetBusVolume("bus:/", volume);
			break;
		}
		case '=':
		{
			// Increase master volume
			float volume = m_AudioSystem->GetBusVolume("bus:/");
			volume = CustomMath::Min(1.0f, volume + 0.1f);
			m_AudioSystem->SetBusVolume("bus:/", volume);
			break;
		}
		case 'm':
			// Toggle music pause state
			m_MusicEvent.SetPaused(!m_MusicEvent.GetPaused());
			break;
		case 'r':
			// Stop or start reverb snapshot
			if (!m_ReverbSnap.IsValid())
			{
				m_ReverbSnap = m_AudioSystem->PlayEvent("snapshot:/WithReverb");
			}
			else
			{
				m_ReverbSnap.Stop();
			}
			break;
		default:
			break;
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

		// Update UI screens
		for (auto ui : m_UIStack)
		{
			if (ui->GetState() == UIScreen::EActive)
			{
				ui->Update(deltaTime);
			}
		}
		// Delete any UIScreens that are closed
		auto iter = m_UIStack.begin();
		while (iter != m_UIStack.end())
		{
			if ((*iter)->GetState() == UIScreen::EClosing)
			{
				delete* iter;
				iter = m_UIStack.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void Game::GenerateOutput()
	{
		m_Renderer->Draw();
	}

	void Game::LoadData()
	{
		// Load English text
		LoadText("src/Assets/UI/English.gptext");

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

		//// Setup floor
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

		////// Forward/back walls
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

		m_FPSActor = new FPSActor(this);
		//m_FollowActor = new FollowActor(this);
		//CameraActor* camera = new CameraActor(this);


		// UI elements
		m_HUD = new HUD(this);

		// Create spheres with audio components playing different sounds
		a = new Actor(this);
		a->SetPosition(Vector3(500.0f, -75.0f, 0.0f));
		a->SetScale(1.0f);
		mc = new MeshComponent(a);
		mc->SetMesh(m_Renderer->GetMesh("src/Assets/3DGraphics/Sphere.gpmesh"));
		AudioComponent* ac = new AudioComponent(a);
		ac->PlayEvent("event:/FireLoop");

		m_MusicEvent = m_AudioSystem->PlayEvent("event:/Music");

		// Create target actors
		a = new TargetActor(this);
		a->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
		a = new TargetActor(this);
		a->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
		a = new TargetActor(this);
		a->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
		a = new TargetActor(this);
		a->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));
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
		while (m_GameState != EQuit)
		{
			ProcessInput();
			UpdateGame();
			GenerateOutput();
		}
	}
}