#pragma once
#include "Actor.h"
#include <vector>

namespace Engine
{
	class Tile : public Actor
	{
	public:
		friend class Grid;
		enum TileState
		{
			EDefault,
			EPath,
			EStart,
			EBase
		};

		Tile(class Game* game);

		void SetTileState(TileState state);
		TileState GetTileState() const { return m_TileState; }
		void ToggleSelect();
		const Tile* GetParent() const { return m_Parent; }
	private:
		// For pathfinding
		std::vector<Tile*> m_Adjacent;
		Tile* m_Parent;
		float f;
		float g;
		float h;
		bool m_InOpenSet;
		bool m_InClosedSet;
		bool m_Blocked;

		void UpdateTexture();
		class SpriteComponent* m_Sprite;
		TileState m_TileState;
		bool m_Selected;
	};
}