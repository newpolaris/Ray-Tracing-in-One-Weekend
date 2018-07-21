#pragma once

#include <glm/glm.hpp>
#include <Math/Ray.h>

namespace Math {

class AABB
{
public:

	AABB(const glm::vec3& a, const glm::vec3& b);

	glm::vec3 min() const;
	glm::vec3 max() const;

	bool hit(const Ray& r, float tmin, float tmax) const;

private:

	glm::vec3 m_Min;
	glm::vec3 m_Max;
};

}
