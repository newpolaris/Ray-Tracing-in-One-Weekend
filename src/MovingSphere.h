#pragma once

#include <Hitable.h>

class MovingSphere : public Hitable 
{
public:

	MovingSphere(const glm::vec3& c0, const glm::vec3& c1, float t0, float t1, float radius, const MaterialPtr& material) noexcept;

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	glm::vec3 center(float time) const;
	
	glm::vec3 m_Center0, m_Center1;
	float m_Time0, m_Time1;
	float m_Radius;
	MaterialPtr m_Material;
};
