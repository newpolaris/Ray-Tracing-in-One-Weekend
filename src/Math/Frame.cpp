#include "Frame.h"

using namespace Math;

Frame::Frame(const glm::vec3& normal)
{
	assert(glm::dot(normal, normal) > 0);

	const auto xaxis = glm::vec3(1, 0, 0);
	const auto yaxis = glm::vec3(0, 1, 0);

	auto n = glm::normalize(normal);
	auto a = glm::abs(n.y) > 0.9f ? xaxis : yaxis;
	auto s = glm::normalize(glm::cross(a, n));
	auto t = glm::cross(n, s);

	m_Axis[0] = s;
	m_Axis[1] = t;
	m_Axis[2] = n;
}

glm::vec3 Frame::local(const glm::vec3& a) const
{
	return a.x * u() + a.y * v() + a.z * w();
}
