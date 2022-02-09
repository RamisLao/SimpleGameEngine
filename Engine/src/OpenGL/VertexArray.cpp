
#include "VertexArray.h"
#include <GL\glew.h>

namespace Engine
{
	VertexArray::VertexArray(const float* verts, unsigned int numVerts,
		const unsigned int* indices, unsigned int numIndices) :
		m_NumVerts(numVerts),
		m_NumIndices(numIndices)
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		// Create vertex buffer
		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(
			GL_ARRAY_BUFFER,				// The active buffer type to write to
			numVerts * 5 * sizeof(float),	// Number of bytes to copy
			verts,							// Source to copy from (pointer)
			GL_STATIC_DRAW					// How will we use this data?
		);

		// Create index buffer
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			numIndices * sizeof(unsigned int),
			indices,
			GL_STATIC_DRAW
		);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,					// Attribute index (0 for first one)
			3,					// Number of components
			GL_FLOAT,			// Type of the components
			GL_FALSE,			// (Only used for integral types)
			sizeof(float) * 5,	// Stride (usually size of each vertex, unless there's padding)
			0					// Offset from start of vertex to this attribute
		);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,			
			2,					// UV
			GL_FLOAT,
			GL_FALSE,
			sizeof(float) * 5,
			reinterpret_cast<void*>(sizeof(float) * 3) //A void pointer is a pointer that has no associated data type with it. A void pointer can hold address of any type and can be typecasted to any type.
		);
	}

	VertexArray::~VertexArray()
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void VertexArray::SetActive()
	{
		glBindVertexArray(m_VertexArray);
	}
}