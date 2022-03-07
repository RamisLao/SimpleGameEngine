#pragma once
#include "Actor.h"
#include "SoundEvent.h"

namespace Engine
{
	class CameraActor : public Actor
	{
	public:
		CameraActor(class Game* game);

		void UpdateActor(float deltaTime) override;

		void SetFootstepSurface(float value);
	private:
		class InputComponent* m_InputComp;
		class AudioComponent* m_AudioComp;
		SoundEvent m_Footstep;
		float m_LastFootstep;
	};
}