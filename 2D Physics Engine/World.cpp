﻿#include "World.h"
#include "AABB.h"
#include "Circle.h"

#include <stdio.h>
#include <glm/ext.hpp>

World::World() = default;
World::~World() = default;

typedef bool(*CollisionFn)(Manifold*);

static CollisionFn CollisionFunctionArray[] =
{
	World::AABBToAABB, World::AABBToCircle, World::CircleToCircle
};

void World::AddActor(Object* Actor)
{
	Actors.emplace_back(Actor);
}

void World::RemoveActor(Object* Actor)
{
	const auto FoundActor = std::find(Actors.begin(), Actors.end(), Actor);

	if (FoundActor < Actors.end())
		Actors.erase(FoundActor);
}

void World::Update(const float DeltaTime)
{
	static float AccumulatedTime = 0.0f;
	AccumulatedTime += DeltaTime;
	
	if (AccumulatedTime >= 0.2f)
		AccumulatedTime = 0.2f;
	
	while (AccumulatedTime >= DeltaTime)
	{
		for (auto Actor : Actors)
			Actor->FixedUpdate(Gravity, TimeStep);
	
		CheckForCollisions();
	
		AccumulatedTime -= TimeStep;
	}
}

void World::UpdateGizmos()
{
	for (auto Actor : Actors)
		Actor->MakeGizmo();
}

void World::CheckForCollisions()
{
	const int ActorCount = Actors.size();

	for (int Outer = 0; Outer < ActorCount - 1; Outer++)
	{
		for (int Inner = Outer + 1; Inner < ActorCount; Inner++)
		{
			Object* Object1 = Actors[Outer];
			Object* Object2 = Actors[Inner];
			auto* M = new Manifold();
			M->A = Object1;
			M->B = Object2;
			const int Shape1 = Object1->GetShape();
			const int Shape2 = Object2->GetShape();

			const unsigned short FunctionIndex = Shape1 + Shape2;
			const CollisionFn CollisionFunctionPtr = CollisionFunctionArray[FunctionIndex];
			
			if (CollisionFunctionPtr != nullptr)
				CollisionFunctionPtr(M);

			//printf("Index: %i\n", FunctionIndex);
		}
	}
}

bool World::AABBToAABB(Manifold* M)
{
	const auto Rec1 = dynamic_cast<AABB*>(M->A);
	const auto Rec2 = dynamic_cast<AABB*>(M->B);

	if (Rec1 != nullptr && Rec2 != nullptr)
	{
		const glm::vec2 Distance1 = Rec2->GetMin() - Rec1->GetMax();
		const glm::vec2 Distance2 = Rec1->GetMin() - Rec2->GetMax();
		const glm::vec2 Distances = glm::vec2(max(Distance1, Distance2));

		const glm::vec2 MaxDistance = max(Distances, Distances);

		if (MaxDistance.x < 0 && MaxDistance.y < 0)
		{
			ResolveCollision(Rec1, Rec2);
			printf("AABBToAABB: Collided!\n");
			return true;
		}

		return false;

		// Exit with no intersection if found separated along each axis
		//if (Rec1->GetMax().x < Rec2->GetMin().x || Rec1->GetMin().x > Rec2->GetMax().x)
		//{
		//	printf("AABB: Not Colliding!\n");
		//	return false;
		//}
		//if (Rec1->GetMax().y < Rec2->GetMin().y || Rec1->GetMin().y > Rec2->GetMax().y)
		//{
		//	printf("AABB: Not Colliding!\n");
		//	return false;
		//}
		//
		//ResolveCollision(Rec1, Rec2);
		//
		//// No separating axis found, at least one overlapping axis is intersecting
		//return true ? printf("AABB: Collided!\n") : false;
	}


	//const glm::vec2 Distance = Rec2->GetLocation() - Rec1->GetLocation();
	//
	//// Calculate half extents along x axis for each object
	//const float Rec1Extent = (Rec1->GetMax().x - Rec1->GetMin().x) / 2;
	//const float Rec2Extent = (Rec2->GetMax().x - Rec2->GetMin().x) / 2;
	//
	//// Calculate overlap on x axis
	//const float XOverlap = Rec1Extent + Rec2Extent - abs(Distance.x);
	//
	//// SAT test on x axis
	//if (XOverlap > 0)
	//{
	//	// Calculate half extents along y axis for each object
	//	const float Rec1Extent2 = (Rec1->GetMax().y - Rec1->GetMin().y) / 2;
	//	const float Rec2Extent2 = (Rec2->GetMax().y - Rec2->GetMin().y) / 2;
	//
	//	// Calculate overlap on y axis
	//	const float YOverlap = Rec1Extent2 + Rec2Extent2 - abs(Distance.y);
	//
	//	if (YOverlap > 0)
	//	{
	//		// Find out which axis is axis of least penetration
	//		if (XOverlap > YOverlap)
	//		{
	//			// Points towards B knowing that Distance from Rec1 to Rec2
	//			if (Distance.x < 0)
	//				M->Normal = glm::vec2(-1.0f, 0.0f);
	//			else
	//				M->Normal = glm::vec2(0.0f, 0.0f);
	//
	//			M->Penetration = XOverlap;
	//
	//			ResolveCollision(Rec1, Rec2);
	//			printf("AABB: Collided!\n");
	//			return true;
	//		}
	//
	//		if (Distance.y < 0)
	//			M->Normal = glm::vec2(0.0f, -1.0f);
	//		else
	//			M->Normal = glm::vec2(0.0f, 1.0f);
	//
	//		M->Penetration = YOverlap;
	//
	//		ResolveCollision(Rec1, Rec2);
	//		printf("AABB: Collided!\n");
	//		return true;
	//	}
	//}

	return false;
}

bool World::CircleToCircle(Manifold* M)
{
	const auto C1 = dynamic_cast<Circle*>(M->A);
	const auto C2 = dynamic_cast<Circle*>(M->B);

	if (C1 != nullptr && C2 != nullptr)
	{
		// The distance between the two circles
		const glm::vec2 Distance = C2->GetLocation() - C1->GetLocation();

		float Radius = C1->GetRadius() + C2->GetRadius();
		Radius *= Radius;

		if (length(Distance) * length(Distance) > Radius)
			return false;

		ResolveCollision(C1, C2);

		return true ? printf("Circle: Collided!\n") : false;
	}

	printf("CircleToCircle: One of the objects were null\n");

	return false;
}

bool World::AABBToCircle(Manifold* M)
{
	const auto Rec = dynamic_cast<AABB*>(M->A);
	const auto Circle = dynamic_cast<::Circle*>(M->B);

	if (Rec != nullptr && Circle != nullptr)
	{
		const glm::vec2 CollisionNormal = Circle->GetLocation() - Rec->GetLocation();

		// Closest Point of Rec to center of Circle
		glm::vec2 Closest = CollisionNormal;

		// Calculate half extents along each axis
		const float XExtent = (Rec->GetMax().x - Rec->GetMin().x) / 2;
		const float YExtent = (Rec->GetMax().y - Rec->GetMin().y) / 2;

		// Clamp point to edges of the AABB
		Closest.x = glm::clamp(-XExtent, XExtent, Closest.x);
		Closest.y = glm::clamp(-YExtent, YExtent, Closest.y);

		bool bInside = false;

		// Circle is inside the AABB, so we need to clamp the circle's center to the closest edge
		if (CollisionNormal == Closest)
		{
			bInside = true;

			// Find the closest axis
			if (fabs(CollisionNormal.x) > fabs(CollisionNormal.y))
			{
				// Clamp to closest extent
				if (Closest.x > 0)
					Closest.x = XExtent;
				else
					Closest.x = -XExtent;
			}
			// y axis is shorter
			else
			{
				// Clamp to the closest extent
				if (Closest.y > 0)
					Closest.y = YExtent;
				else
					Closest.y = -YExtent;
			}
		}

		const glm::vec2 Normal = CollisionNormal - Closest;
		float D = length(Normal) * length(Normal);
		const float Radius = Circle->GetRadius();

		// If the distance is greater than the radius squared, do nothing
		if (D > Radius * Radius && !bInside)
			return false;

		D = sqrtf(D);

		// Collision normal needs to be flipped to point outside if circle was inside the AABB
		if (bInside)
		{
			M->Normal = -CollisionNormal;
			M->Penetration = Radius - D;	
		}
		else
		{
			M->Normal = CollisionNormal;
			M->Penetration = Radius - D;

			ResolveCollision(Rec, Circle);
			printf("AABBToCircle: Collided!\n");
		}

		return true;
	}

	printf("AABBToCircle: One of the objects were null\n");

	return false;
}

float World::Distance(const glm::vec2 A, const glm::vec2 B)
{
	return sqrtf((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

void World::ResolveCollision(Object* const A, Object* const B)
{
	const glm::vec2 RelativeVelocity = B->GetVelocity() - A->GetVelocity();
	const glm::vec2 Normal = B->GetLocation() - A->GetLocation();

	float VelocityAlongNormal = dot(RelativeVelocity, Normal);

	glm::vec2 PreviousVelocity{};

	if (A->GetVelocity() != glm::zero<glm::vec2>())
		PreviousVelocity = A->GetVelocity();

	// Do not resolve if velocities are separating
	if (VelocityAlongNormal > 0)
		return;

	if (B->IsKinematic())
	{
		VelocityAlongNormal = dot(PreviousVelocity, Normal);

		// Calculate restitution
		const float e = glm::min(A->GetRestitution(), B->GetRestitution());

		// Calculate the impulse scalar
		float j = -(1 + e) * VelocityAlongNormal;
		j /= 1.0f / A->GetMass() + 1 / B->GetMass();

		// Calculate the amount of force to apply depending on the object's mass
		const glm::vec2 Impulse = j * Normal;

		// Apply impulse
		A->ApplyForce(A->GetInverseMass()*e * Impulse);
	}

	// Calculate restitution
	const float e = glm::min(A->GetRestitution(), B->GetRestitution());

	// Calculate the impulse scalar
	float j = -(1 + e) * VelocityAlongNormal;
	j /= 1.0f / A->GetMass() + 1 / B->GetMass();

	// Calculate the amount of force to apply depending on the object's mass
	const glm::vec2 Impulse = j * Normal;

	const float MassSum = A->GetMass() + B->GetMass();
	float Ratio = A->GetInverseMass() / MassSum;

	// Apply impulse
	A->ApplyForce(-Ratio*2 * Impulse);
	Ratio = B->GetInverseMass() / MassSum;
	B->ApplyForce(Ratio*2 * Impulse);
	
	PositionalCorrection(A, B);
}

void World::PositionalCorrection(Object* A, Object* B)
{
	const float Percent = 0.2f;
	const float Slop = 0.01f;
	const glm::vec2 Normal = B->GetLocation() - A->GetLocation();
	const float PenetrationDepth = 0.05f;

	const glm::vec2 Correction = glm::max(PenetrationDepth - Slop, 0.0f) / (A->GetInverseMass() + B->GetInverseMass()) * Percent * Normal;

	A->ApplyForce(-A->GetInverseMass() * Correction);
	B->ApplyForce(B->GetInverseMass() * Correction);
}
