#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include <SDL.h>

namespace Engine
{
	DialogBox::DialogBox(Game* game, const std::string& text,
		std::function<void()> onOK)
		:UIScreen(game)
	{
		// Adjust positions for dialog box
		m_BGPos = Vector2(0.0f, 0.0f);
		m_TitlePos = Vector2(0.0f, 100.0f);
		m_NextButtonPos = Vector2(0.0f, 0.0f);

		m_Background = m_Game->GetRenderer()->GetTexture("src/Assets/UI/DialogBG.png");
		SetTitle(text, Vector3::Zero, 30);
		AddButton("OKButton", [onOK]()
			{
				onOK();
			});
		AddButton("CancelButton", [this]()
			{
				Close();
			});
	}

	DialogBox::~DialogBox()
	{
	}
}