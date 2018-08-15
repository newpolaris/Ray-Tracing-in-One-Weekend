#include "Frame.h"

using namespace Math;

Frame::Frame(const glm::vec3& normal)
{
	assert(glm::dot(normal, normal) > 0);

	const auto xaxis = glm::vec3(1, 0, 0);
	const auto yaxis = glm::vec3(0, 1, 0);

	auto n = glm::normalize(normal);
#if 0
	auto a = glm::abs(n.y) > 0.9f ? xaxis : yaxis;
	auto s = glm::normalize(glm::cross(a, n));
	auto t = glm::cross(n, s);
#elif 0
	auto s0 = glm::cross(xaxis, n);
	auto s1 = glm::cross(yaxis, n);
	auto s = glm::normalize(glm::dot(s0, s0) > glm::dot(s1, s1) ? s0 : s1);
	auto t = glm::cross(n, s);
#else
	glm::vec3 a;
	if (glm::abs(n.x) > 0.9f)
		a = glm::vec3(0, 1, 0);
	else
		a = glm::vec3(1, 0, 0);
	auto t = glm::normalize(glm::cross(n, a));
	auto s = glm::cross(n, t);
#endif

	m_Axis[0] = s;
	m_Axis[1] = t;
	m_Axis[2] = n;
}

glm::vec3 Frame::local(const glm::vec3& a) const
{
	return a.x * u() + a.y * v() + a.z * w();
}
