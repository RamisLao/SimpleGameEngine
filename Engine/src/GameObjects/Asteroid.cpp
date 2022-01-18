
#include "Asteroid.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "MoveComponent.h"
#include "CircleComponent.h"

namespace Engine
{
	Asteroid::Asteroid(Game* game) :
		Actor(game)
	{
		// Initialize to random position/orientation
		Vector2 randPos = Random::GetVector(Vector2::Zero,
			Vector2(1024.0f, 768.0f));
		SetPosition(randPos);

		SetRotation(Random::GetFloatRange(0.0f, CustomMath::TwoPi));

		SpriteComponent* sc = new SpriteComponent(this);
		sc->SetTexture(game->GetTexture("src/Assets/Asteroids/Asteroid.png"));

		MoveComponent* mc = new MoveComponent(this);
		mc->SetForwardSpeed(150.0f);

		m_Circle = new CircleComponent(this);
		m_Circle->SetRadius(40.0f);

		game->AddAsteroid(this);
	}

	Asteroid::~Asteroid()
	{
		GetGame()->RemoveAsteroid(this);
	}
}