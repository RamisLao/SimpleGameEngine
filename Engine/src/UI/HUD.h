#pragma once
#include "UIScreen.h"
#include <vector>

namespace Engine
{
	class HUD : public UIScreen
	{
	public:
		// (Lower draw order corresponds with further back)
		HUD(class Game* game);
		~HUD();

		void Update(float deltaTime) override;
		void Draw(class Shader* shader) override;

		void AddTargetComponent(class TargetComponent* tc);
		void RemoveTargetComponent(class TargetComponent* tc);
	protected:
		void UpdateCrosshair(float deltaTime);
		void UpdateRadar(float deltaTime);

		class Texture* m_HealthBar;
		class Texture* m_Radar;
		class Texture* m_Crosshair;
		class Texture* m_CrosshairEnemy;
		class Texture* m_BlipTex;
		class Texture* m_RadarArrow;

		// All the target components in the game
		std::vector<class TargetComponent*> m_TargetComps;
		// 2D offsets of blips relative to radar
		std::vector<Vector2> m_Blips;
		// Adjust range of radar and radius
		float m_RadarRange;
		float m_RadarRadius;
		// Whether the crosshair targets an enemy
		bool m_TargetEnemy;
	};
}