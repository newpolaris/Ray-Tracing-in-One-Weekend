#pragma once

#include <Hitable.h>

class HitableSet : public Hitable
{
public:

	HitableSet();
	HitableSet(const HitableConstIter& begin, const HitableConstIter& end);

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	HitableConstIter m_Begin;
	HitableConstIter m_End;
};
