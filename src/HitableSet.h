#pragma once

#include <Hitable.h>
#include <vector>

class HitableSet : public Hitable
{
public:

	HitableSet();
	HitableSet(const std::vector<HitablePtr>& list);
	HitableSet(std::vector<HitablePtr>&& list) noexcept;

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	std::vector<HitablePtr> m_List;
};
