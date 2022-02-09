#pragma once
#include <vector>
#include "CustomMath.h"
#include <iostream>

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
		void SetPosition(const Vector2& pos) {
			m_Position = pos;
			m_RecomputeWorldTransform = true;
		}
		float GetScale() const { return m_Scale; }
		void SetScale(float scale) {
			m_Scale = scale; 
			m_RecomputeWorldTransform = true;
		}
		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) {
			m_Rotation = rotation; 
			m_RecomputeWorldTransform = true;
		}

		void ComputeWorldTransform();
		const Matrix4& GetWorldTransform() const { return m_WorldTransform; }

		// We don't need to invert the rotation anymore
		Vector2 GetForward() const
		{
			return Vector2(CustomMath::Cos(m_Rotation), CustomMath::Sin(m_Rotation));
		}

		State GetState() const { return m_State; }
		void SetState(State state) { m_State = state; }

		class Game* GetGame() { return m_Game; }

		void AddComponent(class Component* component);
		void RemoveComponent(class Component* component);

	private:
		State m_State;

		// Transform
		// We need a Matrix4 because the vertex layout assumes 3D (x, y, z), even if the z component is not used in 2D
		// Homogenous coordinates for 3D are (x, y, z, w)
		Matrix4 m_WorldTransform;
		Vector2 m_Position;
		float m_Scale;
		float m_Rotation;
		// We recompute the WT only if one of the components changes
		bool m_RecomputeWorldTransform;

		std::vector<class Component*> m_Components;
		class Game* m_Game;
	};
}