#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "FPSActor.h"
#include <algorithm>
#include "TargetComponent.h"

namespace Engine
{
	HUD::HUD(Game* game)
		:UIScreen(game)
		, m_RadarRange(2000.0f)
		, m_RadarRadius(92.0f)
		, m_TargetEnemy(false)
	{
		Renderer* r = m_Game->GetRenderer();
		m_HealthBar = r->GetTexture("src/Assets/UI/HealthBar.png");
		m_Radar = r->GetTexture("src/Assets/UI/Radar.png");
		m_Crosshair = r->GetTexture("src/Assets/UI/Crosshair.png");
		m_CrosshairEnemy = r->GetTexture("src/Assets/UI/CrosshairRed.png");
		m_BlipTex = r->GetTexture("src/Assets/UI/Blip.png");
		m_RadarArrow = r->GetTexture("src/Assets/UI/RadarArrow.png");
	}

	HUD::~HUD()
	{
	}

	void HUD::Update(float deltaTime)
	{
		UIScreen::Update(deltaTime);

		UpdateCrosshair(deltaTime);
		UpdateRadar(deltaTime);
	}

	void HUD::Draw(Shader* shader)
	{
		// Crosshair
		Texture* cross = m_TargetEnemy ? m_CrosshairEnemy : m_Crosshair;
		DrawTexture(shader, cross, Vector2::Zero, 2.0f);

		// Radar
		const Vector2 cRadarPos(-390.0f, 275.0f);
		DrawTexture(shader, m_Radar, cRadarPos, 1.0f);
		// Blips
		for (Vector2& blip : m_Blips)
		{
			DrawTexture(shader, m_BlipTex, cRadarPos + blip, 1.0f);
		}
		// Radar arrow
		DrawTexture(shader, m_RadarArrow, cRadarPos);

		//// Health bar
		//DrawTexture(shader, mHealthBar, Vector2(-350.0f, -350.0f));
	}

	void HUD::AddTargetComponent(TargetComponent* tc)
	{
		m_TargetComps.emplace_back(tc);
	}

	void HUD::RemoveTargetComponent(TargetComponent* tc)
	{
		auto iter = std::find(m_TargetComps.begin(), m_TargetComps.end(),
			tc);
		m_TargetComps.erase(iter);
	}

	void HUD::UpdateCrosshair(float deltaTime)
	{
		// Reset to regular cursor
		m_TargetEnemy = false;
		// Make a line segment
		const float cAimDist = 5000.0f;
		Vector3 start, dir;
		m_Game->GetRenderer()->GetScreenDirection(start, dir);
		LineSegment l(start, start + dir * cAimDist);
		// Segment cast
		PhysWorld::CollisionInfo info;
		if (m_Game->GetPhysWorld()->SegmentCast(l, info))
		{
			// Is this a target?
			for (auto tc : m_TargetComps)
			{
				if (tc->GetOwner() == info.m_Actor)
				{
					m_TargetEnemy = true;
					break;
				}
			}
		}
	}

	void HUD::UpdateRadar(float deltaTime)
	{
		// Clear blip positions from last frame
		m_Blips.clear();

		// Convert player position to radar coordinates (x forward, z up)
		Vector3 playerPos = m_Game->GetPlayer()->GetPosition();
		Vector2 playerPos2D(playerPos.y, playerPos.x);
		// Ditto for player forward
		Vector3 playerForward = m_Game->GetPlayer()->GetForward();
		Vector2 playerForward2D(playerForward.x, playerForward.y);

		// Use atan2 to get rotation of radar
		float angle = CustomMath::Atan2(playerForward2D.y, playerForward2D.x);
		// Make a 2D rotation matrix
		Matrix3 rotMat = Matrix3::CreateRotation(angle);

		// Get positions of blips
		for (auto tc : m_TargetComps)
		{
			Vector3 targetPos = tc->GetOwner()->GetPosition();
			Vector2 actorPos2D(targetPos.y, targetPos.x);

			// Calculate vector between player and target
			Vector2 playerToTarget = actorPos2D - playerPos2D;

			// See if within range
			if (playerToTarget.LengthSq() <= (m_RadarRange * m_RadarRange))
			{
				// Convert playerToTarget into an offset from
				// the center of the on-screen radar
				Vector2 blipPos = playerToTarget;
				blipPos *= m_RadarRadius / m_RadarRange;

				// Rotate blipPos
				blipPos = Vector2::Transform(blipPos, rotMat);
				m_Blips.emplace_back(blipPos);
			}
		}
	}
}