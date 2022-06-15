
#include "Texture.h"
// Simple OpenGL Image Library
#include "SOIL\SOIL.h"

#include "GL\glew.h"

namespace Engine
{
	Texture::Texture() :
		m_TextureID(0),
		m_Width(0),
		m_Height(0)
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Load(const std::string& fileName)
	{
		int channels = 0;
		unsigned char* image = SOIL_load_image(
			fileName.c_str(),		// Name of file
			&m_Width,				// Stores width
			&m_Height,				// Stores height
			&channels,				// Stores number of channels
			SOIL_LOAD_AUTO			// Type of image file, or auto for any
		);

		if (image == nullptr)
		{
			SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
			SOIL_free_image_data(image);
			return false;
		}

		int format = GL_RGB;
		if (channels == 4)
		{
			format = GL_RGBA;
		}

		glGenTextures(1, &m_TextureID);
		// Activate the texture
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		// Copy the raw image data into the texture
		glTexImage2D(
			GL_TEXTURE_2D,		// Texture target
			0,					// Level of detail (for now, assume 0)
			format,				// Color format OpenGL should use
			m_Width,			
			m_Height,
			0,					// Border "this value must be 0"
			format,				// Color format of input data
			GL_UNSIGNED_BYTE,	// Bit depth of input data
								// Unsigned byte specifies 8-bit channels
			image				// Pointer to image data
		);

		// Once you have copied the image data to OpenGL, free the image from memory
		SOIL_free_image_data(image);

		// Enable bilinear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return true;
	}

	void Texture::Unload()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void Texture::CreateFromSurface(SDL_Surface* surface)
	{
		m_Width = surface->w;
		m_Height = surface->h;

		// Generate a GL texture
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_BGRA,
			GL_UNSIGNED_BYTE, surface->pixels);

		// Use linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Texture::SetActive()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}
}