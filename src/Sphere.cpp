#include "Sphere.h"
#include <Math/Intersection.h>

Sphere::Sphere(const glm::vec3& center, float radius, const MaterialPtr& material) noexcept
	: m_Center(center)
	, m_Radius(radius)
	, m_Material(material)
{
}

Sphere::~Sphere()
{
}

bool Sphere::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const 
{
	glm::vec2 t(0.f);

	bool bHit = Math::hitSphere(r.origin(), r.direction(), m_Center, m_Radius, t);
	if (!bHit) return false;

	if (t.x > tMin && t.x < tMax)
	{
		rec.t = t.x;
		rec.position = r.position(rec.t);
		rec.normal = (rec.position - m_Center) / m_Radius;
		rec.material = m_Material;

		return true;
	}
	else if (t.y > tMin && t.y < tMax)
	{
		rec.t = t.y;
		rec.position = r.position(rec.t);
		rec.normal = (rec.position - m_Center) / m_Radius;
		rec.material = m_Material;

		return true;
	}
	return false;
}

bool Sphere::boundingBox(float /*t0*/, float /*t1*/, Math::AABB& box) const
{
	box = Math::AABB(m_Center - glm::vec3(m_Radius), m_Center + glm::vec3(m_Radius));

	return true;
}
