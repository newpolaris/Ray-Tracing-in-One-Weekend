#include <Math/Intersection.h>

namespace Math {
	bool hitSphere(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& c, float r, glm::vec2& t)
	{
		glm::vec3 tc = c - pos;

		float l = glm::dot(tc, dir);
		float d = l*l - glm::dot(tc, tc) + r*r;
		if (d < 0) return false;
		float sl = glm::sqrt(d);

		t = glm::vec2(l - sl, l + sl);
		return true;
	}
}
