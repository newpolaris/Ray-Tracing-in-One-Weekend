#pragma once

#include <Hitable.h>
#include <vector>

class BvhNode : public Hitable
{
public:

	BvhNode();
	BvhNode(const HitableIter& begin, const HitableIter& end, float t0, float t1);
	BvhNode(const std::vector<HitablePtr>& list, float t0, float t1);
	BvhNode(std::vector<HitablePtr>&& list, float t0, float t1) noexcept;

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

    void build(const HitableIter& begin, const HitableIter& end, float t0, float t1);

	float m_Time0, m_Time1;
    std::vector<HitablePtr> m_List;
	HitablePtr m_Left;
	HitablePtr m_Right;
	Math::AABB m_BoundingBox;
};
