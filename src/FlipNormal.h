#pragma once

#include <Types.h>
#include <Hitable.h>
#include <Math/Ray.h>

class FlipNormal : public Hitable
{
public:

	FlipNormal(const HitablePtr& ptr) : m_Ptr(ptr) {}

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const
	{
		if (!m_Ptr->hit(r, tMin, tMax, rec))
			return false;
		rec.normal = -rec.normal;
		return true;
	}

	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const
	{
		return m_Ptr->boundingBox(t0, t0, box);
	}

private:

	HitablePtr m_Ptr;
};
