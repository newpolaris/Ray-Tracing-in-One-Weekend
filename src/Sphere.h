#pragma once

#include "Hitable.h"
#include <Types.h>

class Sphere : public Hitable
{
public:

	Sphere(const glm::vec3& center, float radius, const MaterialPtr& material) noexcept;

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;

private:
	
	glm::vec3 m_Center;
	float m_Radius;
	MaterialPtr m_Material;
};
