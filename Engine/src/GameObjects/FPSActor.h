#pragma once
#include "Actor.h"
#include "SoundEvent.h"

namespace Engine
{
	class FPSActor : public Actor
	{
	public:
		FPSActor(class Game* game);

		void UpdateActor(float deltaTime) override;
		void ActorInput(const struct InputState& state) override;

		void SetFootstepSurface(float value);

		void SetVisible(bool visible);
	private:
		class MoveComponent* m_MoveComp;
		class AudioComponent* m_AudioComp;
		class MeshComponent* m_MeshComp;
		class FPSCamera* m_CameraComp;
		class Actor* m_FPSModel;
		SoundEvent m_Footstep;
		float m_LastFootstep;
	};
}