#include "UIScreen.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Font.h"

namespace Engine
{
	UIScreen::UIScreen(Game* game)
		:m_Game(game)
		, m_Title(nullptr)
		, m_Background(nullptr)
		, m_TitlePos(0.0f, 300.0f)
		, m_NextButtonPos(0.0f, 200.0f)
		, m_BGPos(0.0f, 250.0f)
		, m_State(EActive)
	{
		// Add to UI Stack
		m_Game->PushUI(this);
		m_Font = m_Game->GetFont("src/Assets/UI/Carlito-Regular.ttf");
		m_ButtonOn = m_Game->GetRenderer()->GetTexture("src/Assets/UI/ButtonYellow.png");
		m_ButtonOff = m_Game->GetRenderer()->GetTexture("src/Assets/UI/ButtonBlue.png");
	}

	UIScreen::~UIScreen()
	{
		if (m_Title)
		{
			m_Title->Unload();
			delete m_Title;
		}

		for (auto b : m_Buttons)
		{
			delete b;
		}
		m_Buttons.clear();
	}

	void UIScreen::Update(float deltaTime)
	{

	}

	void UIScreen::Draw(Shader* shader)
	{
		// Draw background (if exists)
		if (m_Background)
		{
			DrawTexture(shader, m_Background, m_BGPos);
		}
		// Draw title (if exists)
		if (m_Title)
		{
			DrawTexture(shader, m_Title, m_TitlePos);
		}
		// Draw buttons
		for (auto b : m_Buttons)
		{
			// Draw background of button
			Texture* tex = b->GetHighlighted() ? m_ButtonOn : m_ButtonOff;
			DrawTexture(shader, tex, b->GetPosition());
			// Draw text of button
			DrawTexture(shader, b->GetNameTex(), b->GetPosition());
		}
		// Override in subclasses to draw any textures
	}

	void UIScreen::ProcessInput(const uint8_t* keys)
	{
		// Do we have buttons?
		if (!m_Buttons.empty())
		{
			// Get position of mouse
			int x, y;
			SDL_GetMouseState(&x, &y);
			// Convert to (0,0) center coordinates
			Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
			mousePos.x -= m_Game->GetRenderer()->GetScreenWidth() * 0.5f;
			mousePos.y = m_Game->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;

			// Highlight any buttons
			for (auto b : m_Buttons)
			{
				if (b->ContainsPoint(mousePos))
				{
					b->SetHighlighted(true);
				}
				else
				{
					b->SetHighlighted(false);
				}
			}
		}
	}

	void UIScreen::HandleKeyPress(int key)
	{
		switch (key)
		{
		case SDL_BUTTON_LEFT:
			if (!m_Buttons.empty())
			{
				for (auto b : m_Buttons)
				{
					if (b->GetHighlighted())
					{
						b->OnClick();
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}

	void UIScreen::Close()
	{
		m_State = EClosing;
	}

	void UIScreen::SetTitle(const std::string& text,
		const Vector3& color,
		int pointSize)
	{
		// Clear out previous title texture if it exists
		if (m_Title)
		{
			m_Title->Unload();
			delete m_Title;
			m_Title = nullptr;
		}
		m_Title = m_Font->RenderText(text, color, pointSize);
	}

	void UIScreen::AddButton(const std::string& name, std::function<void()> onClick)
	{
		Vector2 dims(static_cast<float>(m_ButtonOn->GetWidth()),
			static_cast<float>(m_ButtonOn->GetHeight()));
		Button* b = new Button(name, m_Font, onClick, m_NextButtonPos, dims);
		m_Buttons.emplace_back(b);

		// Update position of next button
		// Move down by height of button plus padding
		m_NextButtonPos.y -= m_ButtonOff->GetHeight() + 20.0f;
	}

	void UIScreen::DrawTexture(class Shader* shader, class Texture* texture,
		const Vector2& offset, float scale)
	{
		// Scale the quad by the width/height of texture
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(texture->GetWidth()) * scale,
			static_cast<float>(texture->GetHeight()) * scale,
			1.0f);
		// Translate to position on screen
		Matrix4 transMat = Matrix4::CreateTranslation(
			Vector3(offset.x, offset.y, 0.0f));
		// Set world transform
		Matrix4 world = scaleMat * transMat;
		shader->SetMatrixUniform("uWorldTransform", world);
		// Set current texture
		texture->SetActive();
		// Draw quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void UIScreen::SetRelativeMouseMode(bool relative)
	{
		if (relative)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
			// Make an initial call to get relative to clear out
			SDL_GetRelativeMouseState(nullptr, nullptr);
		}
		else
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}

	Button::Button(const std::string& name, Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims)
		:m_OnClick(onClick)
		, m_NameTex(nullptr)
		, m_Font(font)
		, m_Position(pos)
		, m_Dimensions(dims)
		, m_Highlighted(false)
	{
		SetName(name);
	}

	Button::~Button()
	{
		if (m_NameTex)
		{
			m_NameTex->Unload();
			delete m_NameTex;
		}
	}

	void Button::SetName(const std::string& name)
	{
		m_Name = name;

		if (m_NameTex)
		{
			m_NameTex->Unload();
			delete m_NameTex;
			m_NameTex = nullptr;
		}
		m_NameTex = m_Font->RenderText(m_Name);
	}

	bool Button::ContainsPoint(const Vector2& pt) const
	{
		bool no = pt.x < (m_Position.x - m_Dimensions.x / 2.0f) ||
			pt.x >(m_Position.x + m_Dimensions.x / 2.0f) ||
			pt.y < (m_Position.y - m_Dimensions.y / 2.0f) ||
			pt.y >(m_Position.y + m_Dimensions.y / 2.0f);
		return !no;
	}

	void Button::OnClick()
	{
		// Call attached handler, if it exists
		if (m_OnClick)
		{
			m_OnClick();
		}
	}
}