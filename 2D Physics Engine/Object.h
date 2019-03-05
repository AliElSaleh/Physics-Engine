#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class Box;

static const float MIN_LINEAR_THRESHOLD = 0.1f;
static const float MIN_ROTATION_THRESHOLD = 0.01f;

#define DEG2RAD(x) ((x) * 0.0174533f)

enum Geometry
{
	AABB, OBB, CIRCLE, PLANE, LAST
};

struct Interval
{
	float Min, Max;
};

class Object
{
public:
	Object();
	virtual ~Object() = default;

	void ApplyForce(glm::vec2 Force);

	virtual void FixedUpdate(glm::vec2 Gravity, float TimeStep);
	virtual void Debug() = 0;
	virtual void MakeGizmo() = 0;

	glm::vec2 GetLocation() const { return Location; }
	glm::vec2 GetVelocity() const { return  Velocity; }
	glm::vec2 GetNormal() const { return Normal; }

	void SetKinematic(const bool State) { bIsKinematic = State; }
	bool IsKinematic() const { return bIsKinematic; }

	Geometry GetShape() const { return Shape; }
	glm::vec4 GetColor() const { return Color; }

	float GetRotation() const { return Rotation; }
	float GetMass() const { return Mass; }
	float GetInverseMass() const { return InverseMass; }
	float GetRestitution() const { return Restitution; }
	float GetAngularVelocity() const { return AngularVelocity; }
	float GetMoment() const { return Moment; }
	float GetFriction() const { return Friction; }

protected:
	float Rotation{0.0f};
	float Mass{1.0f};
	float InverseMass{ 1.0f / Mass };
	float Restitution{1.0f};
	float LinearDrag{0.3f};
	float AngularDrag{0.3f};
	float AngularVelocity{0.0f};
	float Moment{1.0f};
	float InverseMoment{ 1.0f / Moment };
	float Torque{0.0f};
	float Friction{0.7f};

	glm::vec2 Location{};
	glm::vec2 Velocity{1.0f, 1.0f};
	glm::vec2 Normal{};

	glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

	Geometry Shape{};

	bool bIsKinematic{false};
};

