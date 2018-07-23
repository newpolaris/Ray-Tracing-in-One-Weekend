#include <Math/AABB.h>
#include <Math/Ray.h>
#include <algorithm>

namespace Math
{
	AABB Union(const AABB& b0, const AABB& b1)
	{
		glm::vec3 lower = glm::min(b0.min(), b1.min());
		glm::vec3 upper = glm::max(b0.max(), b1.max());
		return AABB(lower, upper);
	}
}

using namespace Math;

AABB::AABB()
{
}

AABB::AABB(const glm::vec3& a, const glm::vec3& b)
	: m_Min(a)
	, m_Max(b)
{
}

glm::vec3 AABB::min() const
{
	return m_Min;
}

glm::vec3 AABB::max() const
{
	return m_Max;
}

// Andrew Kensler's code
bool AABB::hit(const Ray& r, float tmin, float tmax) const
{
	const auto& o = r.origin();
	const auto& d = r.direction();

	for (int i = 0; i < 3; i++)
	{
		auto invD = 1.f / d[i];
		auto t0  = (m_Min[i] - o[i]) * invD;
		auto t1 = (m_Max[i] - o[i]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax <= tmin)
			return false;
	}
	return true;
}
