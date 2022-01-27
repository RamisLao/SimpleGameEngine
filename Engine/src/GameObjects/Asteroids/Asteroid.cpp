
#include "Asteroid.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "MoveComponent.h"
#include "CircleComponent.h"

namespace Engine
{
	Asteroid::Asteroid(Game* game) :
		Actor(game),
		m_Speed(20000.0f)
	{
		// Initialize to random position/orientation
		Vector2 randPos = Random::GetVector(Vector2::Zero,
			Vector2(1024.0f, 768.0f));
		SetPosition(randPos);

		SetRotation(Random::GetFloatRange(0.0f, CustomMath::TwoPi));

		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/Asteroids/Asteroid.png"));

		m_Move = new MoveComponent(this);
		m_Move->SetMass(2.0f);
		m_Move->SetMaxVelocity(300.0f);
		m_Move->SetScreenWrap(true);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(40.0f);

		game->AddAsteroid(this);
	}

	Asteroid::~Asteroid()
	{
		GetGame()->RemoveAsteroid(this);
	}

	void Asteroid::UpdateActor(float deltaTime)
	{
		Vector2 force = GetForward() * m_Speed * deltaTime;
		m_Move->AddForce(force);
	}
}