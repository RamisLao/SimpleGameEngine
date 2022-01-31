
#include "Shader.h"
#include <SDL.h>
#include <fstream>
#include <sstream>

namespace Engine
{
	Shader::Shader() :
		m_ShaderProgram(0),
		m_VertexShader(0),
		m_FragShader(0)
	{
	}

	Shader::~Shader()
	{
	}

	bool Shader::Load(const std::string& vertName, const std::string& fragName)
	{
		// Compile vertex and pixel shaders
		if (!CompileShader(vertName, GL_VERTEX_SHADER, m_VertexShader) ||
			!CompileShader(fragName, GL_FRAGMENT_SHADER, m_FragShader))
		{
			return false;
		}

		// Now create a shader program that
		// links together the vertex/frag shaders
		// When it's time to draw an object, OpenGL uses the currently active shader program to render 
		// the triangles
		m_ShaderProgram = glCreateProgram();
		glAttachShader(m_ShaderProgram, m_VertexShader);
		glAttachShader(m_ShaderProgram, m_FragShader);
		glLinkProgram(m_ShaderProgram);

		// Verify that the program linked successfully
		if (!IsValidProgram())
		{
			return false;
		}

		return true;
	}

	void Shader::Unload()
	{
		glDeleteProgram(m_ShaderProgram);
		glDeleteShader(m_VertexShader);
		glDeleteShader(m_FragShader);
	}

	void Shader::SetActive()
	{
		glUseProgram(m_ShaderProgram);
	}

	bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
	{
		// Open file
		std::ifstream shaderFile(fileName);
		if (shaderFile.is_open())
		{
			// Read all the text into a string
			std::stringstream sstream;
			sstream << shaderFile.rdbuf();
			std::string contents = sstream.str();
			const char* contentsChar = contents.c_str();

			// Create a shader of the specified type
			outShader = glCreateShader(shaderType);
			// Set the source characters and try to compile
			glShaderSource(outShader, 1, &(contentsChar), nullptr);
			glCompileShader(outShader);

			if (!IsCompiled(outShader))
			{
				SDL_Log("Failed to compile shader %s", fileName.c_str());
				return false;
			}
		}
		else
		{
			SDL_Log("Shader file not found: %s", fileName.c_str());
			return false;
		}

		return true;
	}

	bool Shader::IsCompiled(GLuint shader)
	{
		GLint status;
		// Query the compile status
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE)
		{
			char buffer[512];
			memset(buffer, 0, 512);
			glGetShaderInfoLog(shader, 511, nullptr, buffer);
			SDL_Log("GLSL Compile Failed:\n%s", buffer);
			return false;
		}

		return true;
	}

	bool Shader::IsValidProgram()
	{
		GLint status;
		// Query the link status
		glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			char buffer[512];
			memset(buffer, 0, 512);
			glGetProgramInfoLog(m_ShaderProgram, 511, nullptr, buffer);
			SDL_Log("GLSL Link Status:\n%s", buffer);
			return false;
		}

		return true;
	}
}