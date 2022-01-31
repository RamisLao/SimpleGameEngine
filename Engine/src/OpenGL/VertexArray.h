#pragma once

namespace Engine
{
	class VertexArray
	{
	public:
		VertexArray(const float* verts, unsigned int numVerts,
			const unsigned int* indices, unsigned int numIndices);
		~VertexArray();

		// Activate this vertex array (so we can draw it)
		void SetActive();

		unsigned int GetNumIndices() const { return m_NumIndices; }
		unsigned int GetNumVerts() const { return m_NumVerts; }
	private:
		// How many vertices in the vertex buffer?
		unsigned int m_NumVerts;
		// How many indices in the index buffer?
		unsigned int m_NumIndices;
		// OpenGL IDs (OpenGL doesn't return pointers, but IDs of objects
		unsigned int m_VertexBuffer;
		unsigned int m_IndexBuffer;
		unsigned int m_VertexArray;
	};
}