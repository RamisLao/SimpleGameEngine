#pragma once
#include "Component.h"

namespace Engine
{
	class TargetComponent : public Component
	{
	public:
		TargetComponent(class Actor* owner);
		~TargetComponent();
	};
}