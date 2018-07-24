#pragma once

#include "Hitable.h"
#include <Types.h>

class Sphere : public Hitable
{
public:

	Sphere(const glm::vec3& center, float radius, const MaterialPtr& material) noexcept;

	virtual ~Sphere();

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:
	
	glm::vec3 m_Center;
	float m_Radius;
	MaterialPtr m_Material;
};
