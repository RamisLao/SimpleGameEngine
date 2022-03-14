
#pragma once
#include <vector>
#include <functional>
#include "CustomMath.h"
#include "Collision.h"

namespace Engine
{
	class PhysWorld
	{
	public:
		PhysWorld(class Game* game);

		// Used to give helpful information about collision results
		struct CollisionInfo
		{
			// Point of collision
			Vector3 m_Point;
			// Normal at collision
			Vector3 m_Normal;
			// Component collided with
			class BoxComponent* m_Box;
			// Owning actor of component
			class Actor* m_Actor;
		};

		// Test a line segment against boxes
		// Returns true if it collides against a box
		bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

		// Tests collisions using naive pairwise
		void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
		// Test collisions using sweep and prune
		void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

		// Add/remove box components from world
		void AddBox(class BoxComponent* box);
		void RemoveBox(class BoxComponent* box);
	private:
		class Game* m_Game;
		std::vector<class BoxComponent*> m_Boxes;
	};
}

