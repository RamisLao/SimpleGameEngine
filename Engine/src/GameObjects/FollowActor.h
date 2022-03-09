#pragma once
#include "Actor.h"

namespace Engine
{
	class FollowActor : public Actor
	{
	public:
		FollowActor(class Game* game);

		void ActorInput(const struct InputState& state) override;

		void SetVisible(bool visible);
	private:
		class InputComponent* m_InputComp;
		class FollowCamera* m_CameraComp;
		class MeshComponent* m_MeshComp;
	};
}