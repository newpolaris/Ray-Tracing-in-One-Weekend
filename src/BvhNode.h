#pragma once

#include <Hitable.h>

class BvhNode : public Hitable
{
public:

	BvhNode();
	BvhNode(const HitableIter& begin, const HitableIter& end, float t0, float t1);

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	float m_Time0, m_Time1;
	HitablePtr m_Left;
	HitablePtr m_Right;
	Math::AABB m_BoundingBox;
};
