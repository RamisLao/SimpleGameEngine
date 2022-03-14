#include "InputSystem.h"
#include <SDL.h>
#include <cstring>

namespace Engine
{
	bool InputSystem::Initialize()
	{
		// Keyboard
		// Assign current state pointer
		m_State.Keyboard.m_CurrState = SDL_GetKeyboardState(NULL);
		// Clear previous state memory
		//  void * memset ( void * ptr, int value, size_t num );
		// Sets the first num bytes of the block of memory pointed by ptr to the specified value(interpreted as an unsigned char)
		memset(m_State.Keyboard.m_PrevState, 0,
			SDL_NUM_SCANCODES);

		// Mouse (just set everything to 0)
		m_State.Mouse.m_CurrButtons = 0;
		m_State.Mouse.m_PrevButtons = 0;

		// Get the connected controller, if it exists
		m_Controller = SDL_GameControllerOpen(0);
		// Initialize controller state
		m_State.Controller.m_IsConnected = (m_Controller != nullptr);
		memset(m_State.Controller.m_CurrButtons, 0,
			SDL_CONTROLLER_BUTTON_MAX);
		memset(m_State.Controller.m_PrevButtons, 0,
			SDL_CONTROLLER_BUTTON_MAX);

		return true;
	}

	void InputSystem::Shutdown()
	{
	}

	void InputSystem::PrepareForUpdate()
	{
		// Keyboard
		// Prepare for Update is called before SDL_PollEvents, so m_CurrState still has the state from the current frame
		memcpy(m_State.Keyboard.m_PrevState,
			m_State.Keyboard.m_CurrState,
			SDL_NUM_SCANCODES);

		// Mouse
		m_State.Mouse.m_PrevButtons = m_State.Mouse.m_CurrButtons;
		// The mouse wheel event only triggers on frames where the scroll wheel moves
		m_State.Mouse.m_ScrollWheel = Vector2::Zero;

		// Controller
		memcpy(m_State.Controller.m_PrevButtons,
			m_State.Controller.m_CurrButtons,
			SDL_CONTROLLER_BUTTON_MAX);
	}

	void InputSystem::Update()
	{
		// Mouse
		int x = 0, y = 0;
		if (m_State.Mouse.m_IsRelative)
		{
			m_State.Mouse.m_CurrButtons =
				SDL_GetRelativeMouseState(&x, &y);
		}
		else
		{
			// The return value for GetMouseState is a bitmask of the button state
			m_State.Mouse.m_CurrButtons =
				SDL_GetMouseState(&x, &y);
		}

		m_State.Mouse.m_MousePos.x = static_cast<float>(x);
		m_State.Mouse.m_MousePos.y = static_cast<float>(y);

		// Controller
		// Buttons
		for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
		{
			m_State.Controller.m_CurrButtons[i] =
				SDL_GameControllerGetButton(m_Controller,
					SDL_GameControllerButton(i));
		}

		// Triggers
		m_State.Controller.m_LeftTrigger =
			Filter1D(SDL_GameControllerGetAxis(m_Controller,
				SDL_CONTROLLER_AXIS_TRIGGERLEFT));
		m_State.Controller.m_RightTrigger =
			Filter1D(SDL_GameControllerGetAxis(m_Controller,
				SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

		// Sticks
		// We negate the y value because +y in SDL is down
		x = SDL_GameControllerGetAxis(m_Controller,
			SDL_CONTROLLER_AXIS_LEFTX);
		y = -SDL_GameControllerGetAxis(m_Controller,
			SDL_CONTROLLER_AXIS_LEFTY);
		m_State.Controller.m_LeftStick = Filter2D(x, y);

		x = SDL_GameControllerGetAxis(m_Controller,
			SDL_CONTROLLER_AXIS_RIGHTX);
		y = -SDL_GameControllerGetAxis(m_Controller,
			SDL_CONTROLLER_AXIS_RIGHTY);
		m_State.Controller.m_RightStick = Filter2D(x, y);
	}

	void InputSystem::ProcessEvent(SDL_Event& event)
	{
		switch (event.type)
		{
		case SDL_MOUSEWHEEL:
			m_State.Mouse.m_ScrollWheel = Vector2(
				static_cast<float>(event.wheel.x),
				static_cast<float>(event.wheel.y));
			break;
		default:
			break;
		}
	}

	void InputSystem::SetRelativeMouseMode(bool value)
	{
		SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
		// When true, SDL hides the mouse, locks the mouse to the window, and centers the mouse on every frame
		SDL_SetRelativeMouseMode(set);

		m_State.Mouse.m_IsRelative = value;
	}

	float InputSystem::Filter1D(int input)
	{
		// For triggers, the value ranges from 0 to 32,767
		// A value < dead zone is interpreted as 0%
		const int deadZone = 250;
		// A value > max value is interpreted as 100%
		const int maxValue = 30000;

		float retVal = 0.0f;

		// Take absolute value of input
		int absValue = input > 0 ? input : -input;
		// Ignore input within dead zone
		if (absValue > deadZone)
		{
			// Compute fractional value between dead zone and max value
			retVal = static_cast<float>(absValue - deadZone) /
				(maxValue - deadZone);
			// Make sure sign matches original value
			retVal = input > 0 ? retVal : -1.0f * retVal;
			// Clamp between -1.0f and 1.0f
			retVal = CustomMath::Clamp(retVal, -1.0f, 1.0f);
		}

		return retVal;
	}

	Vector2 InputSystem::Filter2D(int inputX, int inputY)
	{
		// For analog sticks axes, the value ranges from  -32,768 to 32,767	
		const float deadZone = 8000.0f;
		const float maxValue = 30000.0f;

		// Make into 2D vector
		Vector2 dir;
		dir.x = static_cast<float>(inputX);
		dir.y = static_cast<float>(inputY);

		float length = dir.Length();

		// If length < deadZone, should be no input
		if (length < deadZone)
		{
			dir = Vector2::Zero;
		}
		else
		{
			// Calculate fractional value between
			// dead zone and max value circles
			float f = (length - deadZone) / (maxValue - deadZone);
			// Clamp f between 0.0f and 1.0f
			f = CustomMath::Clamp(f, 0.0f, 1.0f);
			// Normalize the vector, and then scale it to the
			// fractional value
			dir *= f / length;
		}

		return dir;
	}

	bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
	{
		return m_CurrState[keyCode] == 1;
	}

	ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
	{
		if (m_PrevState[keyCode] == 0)
		{
			if (m_CurrState[keyCode] == 0)
			{
				return ENone;
			}
			else
			{
				return EPressed;
			}
		}
		else // Prev state must be 1
		{
			if (m_CurrState[keyCode] == 0)
			{
				return EReleased;
			}
			else
			{
				return EHeld;
			}
		}
	}

	bool MouseState::GetButtonValue(int button) const
	{
		// Because m_CurrButtons is a bitmask, we use a bitwise-AND to know if a specific button is up or down
		// The SDL_BUTTON macro shifts a bit based on the requested button, and the bitwise-AND return 1 if the button is down
		// and 0 if the button is up
		return (SDL_BUTTON(button) & m_CurrButtons) == 1;
	}

	ButtonState MouseState::GetButtonState(int button) const
	{
		int mask = SDL_BUTTON(button);
		if ((mask & m_PrevButtons) == 0)
		{
			if ((mask & m_CurrButtons) == 0)
			{
				return ENone;
			}
			else
			{
				return EPressed;
			}
		}
		else
		{
			if ((mask & m_CurrButtons) == 0)
			{
				return EReleased;
			}
			else
			{
				return EHeld;
			}
		}
	}
	bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
	{
		return m_CurrButtons[button] == 1;
	}
	ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
	{
		if (m_PrevButtons[button] == 0)
		{
			if (m_CurrButtons[button] == 0)
			{
				return ENone;
			}
			else
			{
				return EPressed;
			}
		}
		else // Prev state must be 1
		{
			if (m_CurrButtons[button] == 0)
			{
				return EReleased;
			}
			else
			{
				return EHeld;
			}
		}
	}
}

