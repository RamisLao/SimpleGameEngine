#include "PauseMenu.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL.h>

namespace Engine
{
	PauseMenu::PauseMenu(Game* game)
		:UIScreen(game)
	{
		m_Game->SetState(Game::EPaused);
		SetRelativeMouseMode(false);
		SetTitle("PauseTitle");
		AddButton("ResumeButton", [this]()
			{
				Close();
			});
		AddButton("QuitButton", [this]()
			{
				new DialogBox(m_Game, "QuitText",
					[this]()
					{
						m_Game->SetState(Game::EQuit);
					});
			});
	}

	PauseMenu::~PauseMenu()
	{
		SetRelativeMouseMode(true);
		m_Game->SetState(Game::EGameplay);
	}

	void PauseMenu::HandleKeyPress(int key)
	{
		UIScreen::HandleKeyPress(key);

		if (key == SDLK_ESCAPE)
		{
			Close();
		}
	}
}