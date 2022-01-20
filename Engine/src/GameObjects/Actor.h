#pragma once
#include <vector>
#include "CustomMath.h"

namespace Engine
{
	class Actor
	{
	public:
		enum State
		{
			EActive,
			EPaused,
			EInvisible,
			EDead
		};

		Actor(class Game* game);
		virtual ~Actor();

		// Update function called from Game (not overridable)
		void Update(float deltaTime);
		void UpdateComponents(float deltaTime);
		// Any actor specific update code (overridable)
		virtual void UpdateActor(float deltaTime);

		// Called from Game (not overridable)
		void ProcessInput(const uint8_t* keyState);
		// Actor-specific input code
		virtual void ActorInput(const uint8_t* keyState);

		// Getters/Setters
		const Vector2& GetPosition() const { return m_Position; }
		void SetPosition(const Vector2& pos) { m_Position = pos; }
		float GetScale() const { return m_Scale; }
		void SetScale(float scale) { m_Scale = scale; }
		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; }

		// In the Unit Circle, +y is up. In SDL, +y is down. So, we need to 
		// invert the sign of the y-component value
		Vector2 GetForward() const
		{
			return Vector2(CustomMath::Cos(m_Rotation), -CustomMath::Sin(m_Rotation));
		}

		State GetState() const { return m_State; }
		void SetState(State state) { m_State = state; }

		class Game* GetGame() { return m_Game; }

		void AddComponent(class Component* component);
		void RemoveComponent(class Component* component);

	private:
		State m_State;

		// Transform
		Vector2 m_Position;
		float m_Scale;
		float m_Rotation;

		std::vector<class Component*> m_Components;
		class Game* m_Game;
	};
}