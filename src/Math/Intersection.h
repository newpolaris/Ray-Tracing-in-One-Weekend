#pragma once

#include <glm/glm.hpp>

namespace Math
{
	bool hitSphere(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& c, float r, glm::vec2& t);
}
