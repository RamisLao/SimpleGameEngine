#pragma once
#include "Actor.h"
#include <vector>

namespace Engine
{
	class Grid : public Actor
	{
	public:
		Grid(class Game* game);

		void ProcessClick(int x, int y);

		bool FindPath(class Tile* start, class Tile* goal);

		void BuildTower();

		void UpdateActor(float deltaTime) override;

		class Tile* GetStartTile();
		class Tile* GetEndTile();
	private:
		void SelectTile(size_t row, size_t col);
		void UpdatePathTiles(class Tile* start);
		class Tile* m_SelectedTile;

		// 2D vector of tiles in grid
		std::vector<std::vector<class Tile*>> m_Tiles;

		// Time until next enemy
		float m_NextEnemy;

		const size_t NumRows = 7;
		const size_t NumCols = 16;
		// Start y position of top left corner
		const float StartY = 192.0f;
		const float TileSize = 64.0f;
		const float EnemyTime = 1.5f;
	};
}