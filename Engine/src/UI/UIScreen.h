#pragma once
#include "CustomMath.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>

namespace Engine
{
	class Button
	{
	public:
		Button(const std::string& name, class Font* font,
			std::function<void()> onClick,
			const Vector2& pos, const Vector2& dims);
		~Button();

		// Set the name of the button
		void SetName(const std::string& name);

		// Getters/setters
		class Texture* GetNameTex() { return m_NameTex; }
		const Vector2& GetPosition() const { return m_Position; }
		void SetHighlighted(bool sel) { m_Highlighted = sel; }
		bool GetHighlighted() const { return m_Highlighted; }

		// Returns true if the point is within the button's bounds
		bool ContainsPoint(const Vector2& pt) const;
		// Called when button is clicked
		void OnClick();
	private:
		std::function<void()> m_OnClick;
		std::string m_Name;
		class Texture* m_NameTex;
		class Font* m_Font;
		Vector2 m_Position;
		Vector2 m_Dimensions;
		bool m_Highlighted;
	};

	class UIScreen
	{
	public:
		UIScreen(class Game* game);
		virtual ~UIScreen();
		// UIScreen subclasses can override these
		virtual void Update(float deltaTime);
		virtual void Draw(class Shader* shader);
		virtual void ProcessInput(const uint8_t* keys);
		virtual void HandleKeyPress(int key);
		// Tracks if the UI is active or closing
		enum UIState
		{
			EActive,
			EClosing
		};
		// Set state to closing
		void Close();
		// Get state of UI screen
		UIState GetState() const { return m_State; }
		// Change the title text
		void SetTitle(const std::string& text,
			const Vector3& color = Color::White,
			int pointSize = 40);
		// Add a button to this screen
		void AddButton(const std::string& name, std::function<void()> onClick);
	protected:
		// Helper to draw a texture
		void DrawTexture(class Shader* shader, class Texture* texture,
			const Vector2& offset = Vector2::Zero,
			float scale = 1.0f);
		// Sets the mouse mode to relative or not
		void SetRelativeMouseMode(bool relative);
		class Game* m_Game;

		class Font* m_Font;
		class Texture* m_Title;
		class Texture* m_Background;
		class Texture* m_ButtonOn;
		class Texture* m_ButtonOff;

		// Configure positions
		Vector2 m_TitlePos;
		Vector2 m_NextButtonPos;
		Vector2 m_BGPos;

		// State
		UIState m_State;
		// List of buttons
		std::vector<Button*> m_Buttons;
	};
}