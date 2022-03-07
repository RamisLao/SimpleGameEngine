#pragma once
#include "Actor.h"
#include "SoundEvent.h"

namespace Engine
{
	class Camera3rdP : public Actor
	{
	public:
		Camera3rdP(class Game* game);

		void UpdateActor(float deltaTime) override;
		void ActorInput(const struct InputState& state) override;

		void SetFootstepSurface(float value);
		const Vector3& GetCameraPosition() const { return m_CameraPos; }
	private:
		class MoveComponent* m_MoveComp;
		class AudioComponent* m_AudioComp;
		Vector3 m_CameraPos;
		SoundEvent m_Footstep;
		float m_LastFootstep;
	};
}
