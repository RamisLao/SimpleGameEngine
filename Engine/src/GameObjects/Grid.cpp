
#include "Grid.h"
#include "Tile.h"
#include "Tower.h"
#include "Enemy.h"

namespace Engine
{
	Grid::Grid(Game* game) :
		Actor(game),
		m_SelectedTile(nullptr)
	{
		m_Tiles.resize(NumRows);
		for (size_t i = 0; i < m_Tiles.size(); i++)
		{
			m_Tiles[i].resize(NumCols);
		}

		// Create Tiles
		for (size_t i = 0; i < NumRows; i++)
		{
			for (size_t j = 0; j < NumCols; j++)
			{
				m_Tiles[i][j] = new Tile(GetGame());
				m_Tiles[i][j]->SetPosition(Vector2(TileSize / 2.0f + j * TileSize, StartY + i * TileSize));
			}
		}

		GetStartTile()->SetTileState(Tile::EStart);
		GetEndTile()->SetTileState(Tile::EBase);

		// Set up adjacency lists
		for (size_t i = 0; i < NumRows; i++)
		{
			for (size_t j = 0; j < NumCols; j++)
			{
				if (i > 0)
				{
					m_Tiles[i][j]->m_Adjacent.push_back(m_Tiles[i - 1][j]);
				}
				if (i < NumRows - 1)
				{
					m_Tiles[i][j]->m_Adjacent.push_back(m_Tiles[i + 1][j]);
				}
				if (j > 0)
				{
					m_Tiles[i][j]->m_Adjacent.push_back(m_Tiles[i][j - 1]);
				}
				if (j < NumCols - 1)
				{
					m_Tiles[i][j]->m_Adjacent.push_back(m_Tiles[i][j + 1]);
				}
			}
		}

		// Find path (in reverse)
		FindPath(GetEndTile(), GetStartTile());
		UpdatePathTiles(GetStartTile());

		m_NextEnemy = EnemyTime;
	}

	void Grid::ProcessClick(int x, int y)
	{
		y -= static_cast<int>(StartY - TileSize / 2);
		if (y >= 0)
		{
			x /= static_cast<int>(TileSize);
			y /= static_cast<int>(TileSize);
			if (x >= 0 && x < static_cast<int>(NumCols) && y >= 0 && y < static_cast<int>(NumRows))
			{
				SelectTile(y, x);
			}
		}
	}

	bool Grid::FindPath(Tile* start, Tile* goal)
	{
		return true;
	}

	void Grid::BuildTower()
	{
		if (m_SelectedTile && !m_SelectedTile->m_Blocked)
		{
			m_SelectedTile->m_Blocked = true;
			if (FindPath(GetEndTile(), GetStartTile()))
			{
				Tower* t = new Tower(GetGame());
				t->SetPosition(m_SelectedTile->GetPosition());
			}
			else
			{
				// This tower would block the path, so don't allow build
				m_SelectedTile->m_Blocked = false;
				FindPath(GetEndTile(), GetStartTile());
			}
			UpdatePathTiles(GetStartTile());
		}
	}

	void Grid::UpdateActor(float deltaTime)
	{
		Actor::UpdateActor(deltaTime);
	}

	Tile* Grid::GetStartTile()
	{
		return m_Tiles[3][0];
	}

	Tile* Grid::GetEndTile()
	{
		return m_Tiles[3][15];
	}

	void Grid::SelectTile(size_t row, size_t col)
	{
		Tile::TileState tState = m_Tiles[row][col]->GetTileState();
		if (tState != Tile::EStart && tState != Tile::EBase)
		{
			if (m_SelectedTile)
			{
				m_SelectedTile->ToggleSelect();
			}
			m_SelectedTile = m_Tiles[row][col];
			m_SelectedTile->ToggleSelect();
		}
	}

	void Grid::UpdatePathTiles(Tile* start)
	{
		for (size_t i = 0; i < NumRows; i++)
		{
			for (size_t j = 0; j < NumCols; j++)
			{
				if (!(i == GetStartTile()->GetPosition().x &&
					  j == GetStartTile()->GetPosition().y) &&
					!(i == GetEndTile()->GetPosition().x) &&
					  j == GetEndTile()->GetPosition().y)
				{
					m_Tiles[i][j]->SetTileState(Tile::EDefault);
				}
			}
		}
	}
}