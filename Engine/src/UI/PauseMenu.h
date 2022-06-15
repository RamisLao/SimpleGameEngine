#pragma once
#include "UIScreen.h"

namespace Engine
{
	class PauseMenu : public UIScreen
	{
	public:
		PauseMenu(class Game* game);
		~PauseMenu();

		void HandleKeyPress(int key) override;
	};
}