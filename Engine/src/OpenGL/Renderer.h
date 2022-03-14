#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include "CustomMath.h"

struct DirectionalLight
{
	// Direction of light
	Vector3 m_Direction;
	// Diffuse Color
	Vector3 m_DiffuseColor;
	// Specular Color
	Vector3 m_SpecColor;
};

namespace Engine
{
	class Renderer
	{
	public:
		Renderer(class Game* game);
		~Renderer();

		bool Initialize(float screenWidth, float screenHeight);
		void Shutdown();
		void UnloadData();

		void Draw();

		void AddSprite(class SpriteComponent* sprite);
		void RemoveSprite(class SpriteComponent* sprite);

		void AddMeshComp(class MeshComponent* mesh);
		void RemoveMeshComp(class MeshComponent* mesh);

		class Texture* GetTexture(const std::string& fileName);
		class Mesh* GetMesh(const std::string& fileName);

		void SetViewMatrix(const Matrix4& view) { m_View = view; }

		void SetAmbientLight(const Vector3& ambient) { m_AmbientLight = ambient; }
		DirectionalLight& GetDirectionalLight() { return m_DirLight; }

		// Given a screen space point, unprojects it into world space,
		// based on the current 3D view/projection matrices
		// Expected ranges:
		// x = [-screenWidth/2, +screenWidth/2]
		// y = [-screenHeight/2, +screenHeight/2]
		// z = [0, 1) -- 0 is closer to camera, 1 is further
		Vector3 Unproject(const Vector3& screenPoint) const;
		void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;

		float GetScreenWidth() const { return m_ScreenWidth; }
		float GetScreenHeight() const { return m_ScreenHeight; }
	private:
		bool LoadShaders();
		void CreateSpriteVerts();
		void SetLightUniforms(class Shader* shader);
		
		// Map of textures loaded
		std::unordered_map<std::string, class Texture*> m_Textures;
		// Loaded meshes
		std::unordered_map<std::string, class Mesh*> m_Meshes;
		// Sprite components drawn
		std::vector<class SpriteComponent*> m_Sprites;
		// Mesh components drawn
		std::vector<class MeshComponent*> m_MeshComps;

		class Game* m_Game;

		// Sprite shader
		class Shader* m_SpriteShader;
		// Sprite vertex array
		class VertexArray* m_SpriteVerts;

		// Mesh shader
		class Shader* m_MeshShader;

		// View/projection for 3D shaders
		Matrix4 m_View;
		Matrix4 m_Projection;
		// Width/height of screen
		float m_ScreenWidth;
		float m_ScreenHeight;

		// Lighting data
		Vector3 m_AmbientLight;
		DirectionalLight m_DirLight;

		// Window
		SDL_Window* m_Window;
		SDL_GLContext m_Context;
	};
}