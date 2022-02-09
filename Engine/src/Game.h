#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <unordered_map>
#include <vector>
#include <ostream>
#include "CustomMath.h"

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

		class Texture* GetTexture(const std::string& fileName);

		// Game-specific

		// Tower Defense
		class Grid* GetGrid() { return m_Grid; }
		std::vector<class Enemy*>& GetEnemies() { return m_Enemies; }
		class Enemy* GetNearestEnemy(const Vector2& pos);

		// Asteroids
		void AddAsteroid(class Asteroid* ast);
		void RemoveAsteroid(class Asteroid* ast);
		std::vector<class Asteroid*>& GetAsteroids() { return m_Asteroids; }
	private:
		void ProcessInput();
		void UpdateGame();
		void GenerateOutput();
		bool LoadShaders();
		void CreateSpriteVerts();
		void LoadData();
		void UnloadData();

		// Map of textures loaded
		std::unordered_map<std::string, class Texture*> m_Textures;

		bool m_IsRunning;
		Uint32 m_TicksCount;

		SDL_Window* m_Window;
		SDL_GLContext m_Context;
		SDL_Renderer* m_Renderer;

		std::vector<class Actor*> m_Actors;
		std::vector<class Actor*> m_PendingActors;
		// Track if we're updating actors right now
		bool m_UpdatingActors;

		// All the sprite components drawn
		std::vector<class SpriteComponent*> m_Sprites;

		// Sprite Shader
		class Shader* m_SpriteShader;
		// Sprite vertex array
		class VertexArray* m_SpriteVerts;

		/////////////////////////////////////////////////////////////////
		// Game-specific
		// Tower Defense
		std::vector<class Enemy*> m_Enemies;
		class Grid* m_Grid;
		float m_NextEnemy;

		// Asteroids
		class Ship* m_Ship;
		std::vector<class Asteroid*> m_Asteroids;
	};
}