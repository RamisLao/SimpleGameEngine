
#include "Mesh.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>
#include <SDL_log.h>
#include "CustomMath.h"

namespace Engine
{
	Mesh::Mesh() :
		m_VertexArray(nullptr),
		m_Radius(0.0f)
	{
	}

	Mesh::~Mesh()
	{
	}

	bool Mesh::Load(const std::string& fileName, Renderer* renderer)
	{
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			SDL_Log("File not found: Mesh %s", fileName.c_str());
			return false;
		}

		std::stringstream fileStream;
		fileStream << file.rdbuf();
		std::string contents = fileStream.str();
		rapidjson::StringStream jsonStr(contents.c_str());
		rapidjson::Document doc;
		doc.ParseStream(jsonStr);

		if (!doc.IsObject())
		{
			SDL_Log("Mesh %s is not valid json", fileName.c_str());
			return false;
		}

		int ver = doc["version"].GetInt();

		if (ver != 1)
		{
			SDL_Log("Mesh %s not version 1", fileName.c_str());
			return false;
		}

		m_ShaderName = doc["shader"].GetString();
		
		size_t vertSize = 8;

		// Load textures
		const rapidjson::Value& textures = doc["textures"];
		if (!textures.IsArray() || textures.Size() < 1)
		{
			SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
			return false;
		}

		// Load textures
		for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
		{
			// Is this texture already loaded?
			std::string texName = textures[i].GetString();
			std::stringstream nameStream;
			nameStream << "src/Assets/3DGraphics/" << texName;
			Texture* t = renderer->GetTexture(nameStream.str());
			if (t == nullptr)
			{
				t = renderer->GetTexture(nameStream.str());
				if (t == nullptr)
				{
					t = renderer->GetTexture("src/Assets/3DGraphics/Default.png");
				}
			}
			m_Textures.emplace_back(t);
		}

		// Load vertices
		const rapidjson::Value& vertsJson = doc["vertices"];
		if (!vertsJson.IsArray() || vertsJson.Size() < 1)
		{
			SDL_Log("Mesh %s has no vertices", fileName.c_str());
			return false;
		}

		std::vector<float> vertices;
		vertices.reserve(vertsJson.Size() * vertSize);
		m_Radius = 0.0f;
		for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
		{
			// For now, just assume we have 8 elements
			const rapidjson::Value& vert = vertsJson[i];
			if (!vert.IsArray() || vert.Size() != 8)
			{
				SDL_Log("Unexpected vertex format for %s", fileName.c_str());
				return false;
			}

			Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
			m_Radius = CustomMath::Max(m_Radius, pos.LengthSq());

			// Add the floats
			for (rapidjson::SizeType i = 0; i < vert.Size(); i++)
			{
				vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
			}
		}

		m_Radius = CustomMath::Sqrt(m_Radius);

		// Load in the indices
		const rapidjson::Value& indJson = doc["indices"];
		if (!indJson.IsArray() || indJson.Size() < 1)
		{
			SDL_Log("Mesh %s has no indices", fileName.c_str());
			return false;
		}

		std::vector<unsigned int> indices;
		indices.reserve(indJson.Size() * 3);
		for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
		{
			const rapidjson::Value& ind = indJson[i];
			if (!ind.IsArray() || ind.Size() != 3)
			{
				SDL_Log("Invalid indices for %s", fileName.c_str());
				return false;
			}

			indices.emplace_back(ind[0].GetUint());
			indices.emplace_back(ind[1].GetUint());
			indices.emplace_back(ind[2].GetUint());
		}

		// Create a vertex array
		m_VertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
			indices.data(), static_cast<unsigned>(indices.size()));

		return true;
	}

	void Mesh::Unload()
	{
		delete m_VertexArray;
		m_VertexArray = nullptr;
	}

	Texture* Mesh::GetTexture(size_t index)
	{
		if (index < m_Textures.size())
		{
			return m_Textures[index];
		}
		else
		{
			return nullptr;
		}
	}
}