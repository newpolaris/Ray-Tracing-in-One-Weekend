#include <Math/AABB.h>
#include <Math/Ray.h>
#include <algorithm>

using namespace Math;

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
		auto a = (m_Min[i] - o[i]) / d[i];
		auto b = (m_Max[i] - o[i]) / d[i];

		float t0 = std::min(a, b);
		float t1 = std::max(a, b);

		tmin = std::max(t0, tmin);
		tmax = std::max(t1, tmax);

		if (tmax <= tmin)
			return false;
	}
	return true;
}
