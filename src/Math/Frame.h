#pragma once

#include <glm/glm.hpp>

namespace Math
{

class Frame
{
public:

	Frame(const glm::vec3& normal);

	const glm::vec3& u() const noexcept { return m_Axis[0]; }
	const glm::vec3& v() const noexcept { return m_Axis[1]; }
	const glm::vec3& w() const noexcept { return m_Axis[2]; }

	glm::vec3 local(const glm::vec3& v) const;

private:

	glm::vec3 m_Axis[3];
};

}
