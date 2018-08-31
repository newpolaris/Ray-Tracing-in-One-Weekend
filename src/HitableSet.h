#pragma once

#include <Hitable.h>
#include <vector>

class HitableSet : public Hitable
{
public:

	HitableSet();
	HitableSet(std::vector<HitablePtr> list) noexcept;

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;
	virtual float pdf_value(const glm::vec3& origin, const glm::vec3& direction) const override;
	virtual glm::vec3 random(const glm::vec3& origin) const override;

private:

	std::vector<HitablePtr> m_List;
};
