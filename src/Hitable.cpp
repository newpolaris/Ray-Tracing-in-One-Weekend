#include "Hitable.h"

Hitable::~Hitable()
{
}

float Hitable::probability(const glm::vec3& origin, const glm::vec3& direction) const
{
	return 0.f;
}

glm::vec3 Hitable::random(const glm::vec3& origin) const
{
	return glm::vec3(1, 0, 0);
}
