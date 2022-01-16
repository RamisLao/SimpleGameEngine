#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <unordered_map>
#include <vector>
#include <ostream>

namespace Engine
{
	class Game
	{
	public:
		Game();
		bool Initialize();
		void RunLoop();
		void Shutdown();
		
		void AddActor(class Actor* actor);
		void RemoveActor(class Actor* actor);

		void AddSprite(class SpriteComponent* sprite);
		void RemoveSprite(class SpriteComponent* sprite);

		SDL_Texture* GetTexture(const std::string& fileName);
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();
		void LoadData();
		void UnloadData();

		// Map of textures loaded
		std::unordered_map<std::string, SDL_Texture*> m_Textures;

		bool m_IsRunning;
		Uint32 m_TicksCount;

		SDL_Window* m_Window;
		SDL_Renderer* m_Renderer;

		std::vector<class Actor*> m_Actors;
		std::vector<class Actor*> m_PendingActors;
		// Track if we're updating actors right now
		bool m_UpdatingActors;

		// All the sprite components drawn
		std::vector<class SpriteComponent*> m_Sprites;

		// Game-specific
		class Ship* m_Ship;
	};
}