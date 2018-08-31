#include "Sphere.h"
#include <glm/gtc/constants.hpp>
#include <Math/Intersection.h>
#include <type_traits>
#include <Math/Random.h>
#include <Math/Frame.h>

namespace 
{
	void getSphereUV(const glm::vec3& p, float& u, float& v)
	{
		const float pi = glm::pi<float>();

		float phi = std::atan2(p.z, p.x);
		float theta = glm::asin(p.y);

		u = 1 - (phi + pi) / (2*pi);
		v = (theta + pi/2) / pi;
	}


	template <typename T>
	decltype(auto) recip(T&& t)
	{
		typedef typename std::remove_reference<T>::type type;
		return type(1.0) / std::forward<T>(t);
	}

	glm::vec3 random_to_sphere(float radius, float distance_squared)
	{
		const float pi = glm::pi<float>();
		auto r1 = Math::BaseRandom();
		auto r2 = Math::BaseRandom();
		float rr = radius*radius;
		float cos_theta_max = glm::sqrt(glm::max(0.f, 1 - rr*recip(distance_squared)));
		float z = 1 - r2 * (1 - cos_theta_max);
		float phi = 2 * pi * r1;
		float sin_theta = glm::sqrt(glm::max(0.f, 1 - z*z));
		float x = glm::cos(phi) * sin_theta;
		float y = glm::sin(phi) * sin_theta;
		return glm::vec3(x, y, z);
	}
}

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
		getSphereUV(rec.normal, rec.u, rec.v);

		return true;
	}
	else if (t.y > tMin && t.y < tMax)
	{
		rec.t = t.y;
		rec.position = r.position(rec.t);
		rec.normal = (rec.position - m_Center) / m_Radius;
		rec.material = m_Material;
		getSphereUV(rec.normal, rec.u, rec.v);

		return true;
	}
	return false;
}

bool Sphere::boundingBox(float /*t0*/, float /*t1*/, Math::AABB& box) const
{
	box = Math::AABB(m_Center - glm::vec3(m_Radius), m_Center + glm::vec3(m_Radius));

	return true;
}

float Sphere::pdf_value(const glm::vec3& origin, const glm::vec3& direction) const
{
	const float pi = glm::pi<float>();

	HitRecord hrec;
	if (!hit(Math::Ray(origin, direction), 0.001f, FLT_MAX, hrec))
		return 0.f;

	auto dir = m_Center - origin;
	float distance_squared = glm::dot(dir, dir);
	float radius_squared = m_Radius*m_Radius;
	float cos_theta_max = glm::sqrt(glm::max(0.f, 1.f - radius_squared*recip(distance_squared)));
	float solid_angle = 2*pi*(1.f - cos_theta_max);
	return recip(solid_angle);
}

glm::vec3 Sphere::random(const glm::vec3& origin) const
{
	auto direction = m_Center - origin;
	float distance_squared = glm::dot(direction, direction);

	Math::Frame frame(direction);
	return frame.local(random_to_sphere(m_Radius, distance_squared));
}
