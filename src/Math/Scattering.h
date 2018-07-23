#pragma once

#include <glm/glm.hpp>

namespace Math {

	bool refract(const glm::vec3& l, const glm::vec3& n, float r, glm::vec3& refracted);
	float schlick(float cosine, float refractiveIndex);

}
