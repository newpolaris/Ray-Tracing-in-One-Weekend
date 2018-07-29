#pragma once

#include <glm/glm.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/AABB.h>
#include <cmath>

class RotateY : public Hitable
{
public:

	RotateY(const HitablePtr& ptr, float angle);

	virtual bool hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const;

private:

	HitablePtr m_Ptr;
	bool m_bHasBox;
	Math::AABB m_Box;
	float m_SinTheta;
	float m_CosTheta;
};

inline RotateY::RotateY(const HitablePtr& ptr, float angle)
	: m_Ptr(ptr)
{
	float radian = glm::radians(angle);
	m_SinTheta = glm::sin(radian);
	m_CosTheta = glm::cos(radian);

	m_bHasBox = m_Ptr->boundingBox(0, 1, m_Box);

	glm::vec3 Min(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	auto minE = m_Box.min();
	auto maxE = m_Box.max();
	for (int i = 0; i < 2; i++)
	for (int j = 0; j < 2; j++)
	for (int k = 0; k < 2; k++)
	{
		glm::vec3 t(
				i == 0 ? minE.x : maxE.x,
				j == 0 ? minE.y : maxE.y,
				k == 0 ? minE.z : maxE.z);
		float x = +m_CosTheta*t.x + m_SinTheta*t.z;
		float y = t.y;
		float z = -m_SinTheta*t.x + m_CosTheta*t.z;
		glm::vec3 newE(x, y, z);
		Min = glm::min(Min, newE);
		Max = glm::max(Max, newE);
	}
	m_Box = Math::AABB(Min, Max);
}

inline bool RotateY::hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const
{
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = m_CosTheta * r.origin()[0] - m_SinTheta * r.origin()[2];
	origin[2] = m_SinTheta * r.origin()[0] + m_CosTheta * r.origin()[2];
	direction[0] = m_CosTheta * r.direction()[0] - m_SinTheta * r.direction()[2];
	direction[2] = m_SinTheta * r.direction()[0] + m_CosTheta * r.direction()[2];

	Math::Ray rotated_r(origin, direction, r.time());
	if (!m_Ptr->hit(rotated_r, t0, t1, rec))
		return false;

	glm::vec3 p = rec.position;
	glm::vec3 normal = rec.normal;

	p[0] = +m_CosTheta*rec.position[0] + m_SinTheta * rec.position[2];
	p[2] = -m_SinTheta*rec.position[2] + m_CosTheta * rec.position[2];
	normal[0] = +m_CosTheta*rec.normal[0] + m_SinTheta * rec.normal[2];
	normal[2] = -m_SinTheta*rec.normal[2] + m_CosTheta * rec.normal[2];
	rec.position = p;
	rec.normal = normal;

	return true;
}

inline bool RotateY::boundingBox(float t0, float t1, Math::AABB& box) const
{
	box = m_Box;
	return m_bHasBox;
}
