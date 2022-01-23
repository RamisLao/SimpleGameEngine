
#include "Tile.h"
#include "SpriteComponent.h"
#include <string>
#include "Game.h"

namespace Engine
{
	Tile::Tile(Game* game) :
		Actor(game),
		m_Sprite(nullptr),
		m_TileState(EDefault)
	{
		m_Sprite = new SpriteComponent(this);
		UpdateTexture();
	}

	void Tile::SetTileState(TileState state)
	{
		m_TileState = state;
		UpdateTexture();
	}

	void Tile::ToggleSelect()
	{
		m_Selected = !m_Selected;
		UpdateTexture();
	}

	void Tile::UpdateTexture()
	{
		std::string path;
		switch (m_TileState)
		{
		case EStart:
			path = "src/Assets/TowerDefense/TileTan.png";
			break;
		case EBase:
			path = "src/Assets/TowerDefense/TileGreen.png";
			break;
		case EPath:
			if (m_Selected)
				path = "src/Assets/TowerDefense/TileGreySelected.png";
			else
				path = "src/Assets/TowerDefense/TileGrey.png";
			break;
		case EDefault:
		default:
			if (m_Selected)
				path = "src/Assets/TowerDefense/TileBrownSelected.png";
			else
				path = "src/Assets/TowerDefense/TileBrown.png";
			break;
		}

		m_Sprite->SetTexture(GetGame()->GetTexture(path));
	}
}