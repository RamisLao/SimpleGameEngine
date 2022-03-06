#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include "CustomMath.h"
#include <SDL_events.h>

namespace Engine
{
	// The different button states
	enum ButtonState
	{
		ENone,
		EPressed,
		EReleased,
		EHeld
	};

	// Helper for keyboard input
	class KeyboardState
	{
	public:
		// Friend so InputSystem can easily update it
		friend class InputSystem;
		// Get just the boolean true/false value of key
		bool GetKeyValue(SDL_Scancode keyCode) const;
		// Get a state based on current and previous frame
		ButtonState GetKeyState(SDL_Scancode keyCode) const;
	private:
		// Pointer to the keyboard state. This doesn't change throughout the lifetime of the application
		const Uint8* m_CurrState;
		// SDL overwrites the current keyboard state when you call SDL_PollEvents, so you need to store the previous frame state
		// Size of array corresponds to the size of the buffer that SDL uses for keyboard scan codes
		Uint8 m_PrevState[SDL_NUM_SCANCODES];
	};

	// Helper for mouse input
	class MouseState
	{
	public:
		friend class InputSystem;

		// For mouse position
		const Vector2& GetPosition() const { return m_MousePos; }
		const Vector2& GetScrollWheel() const { return m_ScrollWheel; }
		bool IsRelative() const { return m_IsRelative; }

		// For buttons
		bool GetButtonValue(int button) const;
		ButtonState GetButtonState(int button) const;
	private:
		// Store current mouse position
		Vector2 m_MousePos;
		// Motion of scroll wheel
		Vector2 m_ScrollWheel;
		// Store button data
		Uint32 m_CurrButtons;
		Uint32 m_PrevButtons;
		// Are we in relative mouse mode
		// Sometimes we want to know the current coordinates of the mouse
		// Sometimes we want to know the relatice change of the mouse between frames
		// For example, in 1st person shooters, moving the camera with the mouse needs relative movement
		// It is faster if the mouse moves faster and slower if the mouse moves slower
		bool m_IsRelative;
	};

	// Helper for controller input
	class ControllerState
	{
	public:
		friend class InputSystem;

		// For buttons
		bool GetButtonValue(SDL_GameControllerButton button) const;
		ButtonState GetButtonState(SDL_GameControllerButton button) const;

		const Vector2& GetLeftStick() const { return m_LeftStick; }
		const Vector2& GetRightStick() const { return m_RightStick; }
		float GetLeftTrigger() const { return m_LeftTrigger; }
		float GetRightTrigger() const { return m_RightTrigger; }

		bool GetIsConnected() const { return m_IsConnected; }
	private:
		// Current/previous buttons
		Uint8 m_CurrButtons[SDL_CONTROLLER_BUTTON_MAX];
		Uint8 m_PrevButtons[SDL_CONTROLLER_BUTTON_MAX];
		// Left/right sticks
		Vector2 m_LeftStick;
		Vector2 m_RightStick;
		// Left/right trigger
		float m_LeftTrigger;
		float m_RightTrigger;
		// Is this controller connected?
		bool m_IsConnected;
	};

	// Wrapper that contains current state of input
	struct InputState
	{
		KeyboardState Keyboard;
		MouseState Mouse;
		ControllerState Controller;
	};

	class InputSystem
	{
	public:
		bool Initialize();
		void Shutdown();

		// Called right before SDL_PollEvents loop
		void PrepareForUpdate();
		// Called after SDL_PollEvents loop
		void Update();
		// Called to process an SDL event in input system
		void ProcessEvent(SDL_Event& event);

		const InputState& GetState() const { return m_State; }

		void SetRelativeMouseMode(bool value);
	private:
		float Filter1D(int input);
		Vector2 Filter2D(int inputX, int inputY);
		InputState m_State;
		SDL_GameController* m_Controller;
	};
}