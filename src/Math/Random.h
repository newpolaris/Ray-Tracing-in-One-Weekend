#pragma once

#include <glm/glm.hpp>

namespace Math 
{
	float BaseRandom();
	glm::vec2 randomUnitDisk();
	glm::vec3 randomUnitSphere();
	glm::vec3 randomCosineDirection();
	float randomCosineDirectionPdf(float cosTheta);
};
