
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "MeshComponent.h"

namespace Engine
{
	MeshComponent::MeshComponent(Actor* owner) :
		Component(owner),
		m_Mesh(nullptr),
		m_TextureIndex(0)
	{
		m_Owner->GetGame()->GetRenderer()->AddMeshComp(this);
	}

	MeshComponent::~MeshComponent()
	{
		m_Owner->GetGame()->GetRenderer()->RemoveMeshComp(this);
	}

	void MeshComponent::Draw(Shader* shader)
	{
		if (m_Mesh)
		{
			shader->SetMatrixUniform("uWorldTransform",
				m_Owner->GetWorldTransform());

			Texture* t = m_Mesh->GetTexture(m_TextureIndex);
			if (t)
			{
				t->SetActive();
			}

			VertexArray* va = m_Mesh->GetVertexArray();
			va->SetActive();

			glDrawElements(
				GL_TRIANGLES,
				va->GetNumIndices(),
				GL_UNSIGNED_INT,
				nullptr
			);
		}
	}
}