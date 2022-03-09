#pragma once

#include "Component.h"
#include <cstddef>

namespace Engine
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent(class Actor* owner);
		~MeshComponent();

		virtual void Draw(class Shader* shader);
		virtual void SetMesh(class Mesh* mesh) { m_Mesh = mesh; }
		void SetTextureIndex(size_t index) { m_TextureIndex = index; }

		void SetVisible(bool visible) { m_Visible = visible; }
		bool GetVisible() const { return m_Visible; }
	private:
		class Mesh* m_Mesh;
		size_t m_TextureIndex;
		bool m_Visible;
	};
}