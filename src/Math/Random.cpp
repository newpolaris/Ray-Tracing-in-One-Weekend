#include "Random.h"

float Math::BaseRandom()
{
	return drand48();
}

glm::vec3 Math::randomUnitSphere()
{
	glm::vec3 p;
	do {
		p = 2.f * glm::vec3(BaseRandom(), BaseRandom(), BaseRandom()) - 1.f;
	} while (glm::dot(p, p) >= 1.f);
	return p;
}

