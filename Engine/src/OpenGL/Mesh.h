#pragma once
#include <vector>
#include <string>

namespace Engine
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		bool Load(const std::string& fileName, class Renderer* renderer);
		void Unload();
		// Get the vertex array associated with this mesh
		class VertexArray* GetVertexArray() { return m_VertexArray; }
		class Texture* GetTexture(size_t index);
		const std::string& GetShaderName() const { return m_ShaderName; }
		float GetRadius() const { return m_Radius; }
		float GetSpecPower() const { return m_SpecPower; }
	private:
		// Textures associated with this mesh
		std::vector<class Texture*> m_Textures;
		// Vertex array associated with this mesh
		class VertexArray* m_VertexArray;
		// Name of shader specified by mesh
		std::string m_ShaderName;
		// Stores object space bounding sphere radius
		float m_Radius;
		// Specular power of the surface
		float m_SpecPower;
	};
}