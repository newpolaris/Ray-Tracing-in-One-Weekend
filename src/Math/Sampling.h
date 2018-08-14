#pragma once

#include <glm/glm.hpp>

namespace Math 
{
	glm::vec2 uniformSampleDisk(const glm::vec2& u);
	glm::vec2 concentricSampleDisk(const glm::vec2& u);
	glm::vec3 uniformSampleHemisphere(const glm::vec2& u);
	glm::vec3 cosineSampleHemisphere(const glm::vec2& u);
}
