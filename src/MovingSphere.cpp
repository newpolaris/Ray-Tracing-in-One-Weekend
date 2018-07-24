#include <MovingSphere.h>
#include <Math/Intersection.h>

MovingSphere::MovingSphere(const glm::vec3& c0, const glm::vec3& c1, float t0, float t1, float radius, const MaterialPtr& material) noexcept
	: m_Center0(c0)
	, m_Center1(c1)
	, m_Time0(t0)
	, m_Time1(t1)
	, m_Radius(radius)
	, m_Material(material)
{
}

MovingSphere::~MovingSphere()
{
}

bool MovingSphere::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const
{
	glm::vec2 t(0.f);

	auto position = center(r.time());
	bool bHit = Math::hitSphere(r.origin(), r.direction(), position, m_Radius, t);
	if (!bHit) return false;

	if (t.x > tMin && t.x < tMax)
	{
		rec.t = t.x;
		rec.position = r.position(rec.t);
		rec.normal = (rec.position - position) / m_Radius;
		rec.material = m_Material;

		return true;
	}
	else if (t.y > tMin && t.y < tMax)
	{
		rec.t = t.y;
		rec.position = r.position(rec.t);
		rec.normal = (rec.position - position) / m_Radius;
		rec.material = m_Material;

		return true;
	}
	return false;

}

bool MovingSphere::boundingBox(float t0, float t1, Math::AABB& box) const
{
	auto c0 = center(t0);
	auto c1 = center(t1);
	auto minC = glm::min(c0, c1);
	auto maxC = glm::max(c0, c1);
	box = Math::AABB(minC - glm::vec3(m_Radius), maxC + glm::vec3(m_Radius));

	return true;
}

glm::vec3 MovingSphere::center(float time) const
{
	return m_Center0 + ((time - m_Time0) / (m_Time1 - m_Time0))*(m_Center1 - m_Center0);
}
