#pragma once

#include <glm/glm.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/AABB.h>

class Translate : public Hitable
{
public:

	Translate(const HitablePtr& ptr, const glm::vec3& displacement);

	virtual bool hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const;

private:

	HitablePtr m_Ptr;
	glm::vec3 m_Offset;
};


inline Translate::Translate(const HitablePtr& ptr, const glm::vec3& displacement)
	: m_Ptr(ptr)
	, m_Offset(displacement)
{
}

inline bool Translate::hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const
{
	Math::Ray move_r(r.origin() - m_Offset, r.direction(), r.time());
	if (!m_Ptr->hit(move_r, t0, t1, rec))
		return false;
	rec.position += m_Offset;
	return true;
}

inline bool Translate::boundingBox(float t0, float t1, Math::AABB& box) const
{
	if (!m_Ptr->boundingBox(t0, t1, box))
		return false;
	box = Math::AABB(box.min() + m_Offset, box.max() + m_Offset);
	return true;
}
