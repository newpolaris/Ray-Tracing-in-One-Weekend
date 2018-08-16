#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/AABB.h>
#include <cmath>

class Rotate : public Hitable
{
public:

	Rotate(const HitablePtr& ptr, const glm::quat& quat);

	virtual bool hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const;

private:

	HitablePtr m_Ptr;
	bool m_bHasBox;
	Math::AABB m_Box;
	glm::quat m_Rot;
	glm::mat3 m_Rotate;
	glm::mat3 m_Reverse;
};

inline Rotate::Rotate(const HitablePtr& ptr, const glm::quat& quat)
	: m_Ptr(ptr)
{
	m_Rotate = glm::mat3_cast(quat);
	m_Reverse = glm::mat3_cast(glm::conjugate(quat));
	m_bHasBox = m_Ptr->boundingBox(0, 1, m_Box);

	auto edge = std::vector<glm::vec3>({m_Box.min(), m_Box.max()});
	auto corners = {
		glm::vec3(edge[0].x, edge[0].y, edge[0].z),
		glm::vec3(edge[0].x, edge[0].y, edge[1].z),
		glm::vec3(edge[0].x, edge[1].y, edge[0].z),
		glm::vec3(edge[0].x, edge[1].y, edge[1].z),
		glm::vec3(edge[1].x, edge[0].y, edge[0].z),
		glm::vec3(edge[1].x, edge[0].y, edge[1].z),
		glm::vec3(edge[1].x, edge[1].y, edge[0].z),
		glm::vec3(edge[1].x, edge[1].y, edge[1].z),
	};
	glm::vec3 Min(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto p : corners)
	{
		auto rotP = m_Rotate * p;
		Min = glm::min(Min, rotP);
		Max = glm::max(Max, rotP);
	}
	m_Box = Math::AABB(Min, Max);
}

inline bool Rotate::hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const
{
	auto origin = m_Reverse * r.origin();
	auto direction = m_Reverse * r.direction();

	Math::Ray rotated_r(origin, direction, r.time());
	if (!m_Ptr->hit(rotated_r, t0, t1, rec))
		return false;

	glm::vec3 position = m_Rotate * rec.position;
	glm::vec3 normal = m_Rotate * rec.normal;
	rec.position = position;
	rec.normal = glm::normalize(normal);

	return true;
}

inline bool Rotate::boundingBox(float t0, float t1, Math::AABB& box) const
{
	box = m_Box;
	return m_bHasBox;
}

class RotateX : public Rotate
{
public:

	RotateX(const HitablePtr& ptr, float angle) : Rotate(ptr, glm::angleAxis(glm::radians(angle), glm::vec3(1, 0, 0)))
	{
	}
};

class RotateY : public Rotate
{
public:

	RotateY(const HitablePtr& ptr, float angle) : Rotate(ptr, glm::angleAxis(glm::radians(angle), glm::vec3(0, 1, 0)))
	{
	}
};

class RotateZ : public Rotate
{
public:

	RotateZ(const HitablePtr& ptr, float angle) : Rotate(ptr, glm::angleAxis(glm::radians(angle), glm::vec3(0, 0, 1)))
	{
	}
};
