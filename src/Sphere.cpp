#include "Sphere.h"

namespace 
{
	bool hitSphere(glm::vec3 pos, glm::vec3 dir, glm::vec3 c, float r, glm::vec2& t)
	{
		glm::vec3 tc = c - pos;

		float l = glm::dot(tc, dir);
		float d = l*l - glm::dot(tc, tc) + r*r;
		if (d < 0) return false;
		float sl = glm::sqrt(d);

		t = glm::vec2(l - sl, l + sl);
		return true;
	}
}

Sphere::Sphere() noexcept
{
}

Sphere::Sphere(const glm::vec3& center, float radius, const MaterialPtr& material) noexcept
	: m_Center(center)
	, m_Radius(radius)
	, m_Material(material)
{
}

bool Sphere::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const 
{
	glm::vec2 t(0.f);

	bool bHit = hitSphere(r.origin(), r.direction(), m_Center, m_Radius, t);
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
