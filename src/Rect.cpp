#include "Rect.h"


RectXY::RectXY()
{
}

RectXY::RectXY(float x0, float x1, float y0, float y1, float z, const MaterialPtr& material)
	: m_X0(x0), m_X1(x1), m_Y0(y0), m_Y1(y1), m_Z(z), m_Material(material)
{
}

bool RectXY::hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const
{
	const auto& o = ray.origin();
	const auto& d = ray.direction();
	float t = (m_Z - o.z) / d.z;
	if (t < t0 || t > t1)
		return false;

	float x = o.x + t * d.x;
	float y = o.y + t * d.y;
	if (x < m_X0 || x > m_X1 || y < m_Y0 || y > m_Y1)
		return false;

	rec.u = (x - m_X0) / (m_X1 - m_X0);
	rec.v = (y - m_Y0) / (m_Y1 - m_Y0);
	rec.t = t;
	rec.material = m_Material;
	rec.position = ray.position(t);
	rec.normal = glm::vec3(0.f, 0.f, 1.f);

	return true;
}

bool RectXY::boundingBox(float t0, float t1, Math::AABB& box) const
{
	const float planeThickness = 0.0001f;
	box = Math::AABB(glm::vec3(m_X0, m_Y0, m_Z - planeThickness),
					 glm::vec3(m_X1, m_Y1, m_Z + planeThickness));
	return true;
}

RectXZ::RectXZ()
{
}

RectXZ::RectXZ(float x0, float x1, float z0, float z1, float y, const MaterialPtr& material)
	: m_X0(x0), m_X1(x1), m_Z0(z0), m_Z1(z1), m_Y(y), m_Material(material)
{
}

bool RectXZ::hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const
{
	const auto& o = ray.origin();
	const auto& d = ray.direction();
	float t = (m_Y - o.y) / d.y;
	if (t < t0 || t > t1)
		return false;

	float x = o.x + t * d.x;
	float z = o.z + t * d.z;
	if (x < m_X0 || x > m_X1 || z < m_Z0 || z > m_Z1)
		return false;

	rec.u = (x - m_X0) / (m_X1 - m_X0);
	rec.v = (z - m_Z0) / (m_Z1 - m_Z0);
	rec.t = t;
	rec.material = m_Material;
	rec.position = ray.position(t);
	rec.normal = glm::vec3(0.f, 1.f, 0.f);

	return true;
}

bool RectXZ::boundingBox(float t0, float t1, Math::AABB& box) const
{
	const float planeThickness = 0.0001f;
	box = Math::AABB(glm::vec3(m_X0, m_Y - planeThickness, m_Z0),
					 glm::vec3(m_X1, m_Y + planeThickness, m_Z1));
	return true;
}

RectYZ::RectYZ()
{
}

RectYZ::RectYZ(float y0, float y1, float z0, float z1, float x, const MaterialPtr& material)
	: m_Y0(y0), m_Y1(y1), m_Z0(z0), m_Z1(z1), m_X(x), m_Material(material)
{
}

bool RectYZ::hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const
{
	const auto& o = ray.origin();
	const auto& d = ray.direction();
	float t = (m_X - o.x) / d.x;
	if (t < t0 || t > t1)
		return false;

	float y = o.y + t * d.y;
	float z = o.z + t * d.z;
	if (y < m_Y0 || y > m_Y1 || z < m_Z0 || z > m_Z1)
		return false;

	rec.u = (y - m_Y0) / (m_Y1 - m_Y0);
	rec.v = (z - m_Z0) / (m_Z1 - m_Z0);
	rec.t = t;
	rec.material = m_Material;
	rec.position = ray.position(t);
	rec.normal = glm::vec3(0.f, 1.f, 0.f);

	return true;
}

bool RectYZ::boundingBox(float t0, float t1, Math::AABB& box) const
{
	const float planeThickness = 0.0001f;
	box = Math::AABB(glm::vec3(m_X - planeThickness, m_Y0, m_Z0),
					 glm::vec3(m_X + planeThickness, m_Y1, m_Z1));
	return true;
}
