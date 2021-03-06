#pragma once
#include "Object.h"

#include <glm/mat4x4.hpp>

class OBB final : public Object
{
public:
	OBB();
	OBB(glm::vec2 Location, glm::vec2 Velocity, glm::vec2 Extent, float Rotation, float Mass, glm::vec4 Color);
	~OBB();

	void FixedUpdate(glm::vec2 Gravity, float TimeStep) override;
	void Debug() override;
	void MakeGizmo() override;

	glm::vec2 GetExtent() const { return HalfExtent; }

	glm::mat4 GetTransform() const { return Transform; }

private:
	glm::vec2 HalfExtent{};

	glm::mat4 Transform{}; // For rotation
};

