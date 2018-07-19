#pragma once

#include <glm/glm.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/Random.h>

class HitRecord;

class Material
{
public:
	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:

	Lambertian(const glm::vec3& albedo) : m_Albedo(albedo)
	{
	}

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override
	{
		glm::vec3 target = rec.position + rec.normal + Math::randomUnitSphere();
		glm::vec3 dir = glm::normalize(target - rec.position);

		scattered = Math::Ray(rec.position, dir);
		attenuation = m_Albedo;

		return true;
	}

private:

	glm::vec3 m_Albedo;
};

class Metal : public Material
{
public:

	Metal(const glm::vec3& albedo, float fuzz) : m_Albedo(albedo), m_Fuzz(fuzz)
	{
		assert(fuzz <= 1.f);
	}

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override
	{
		glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);
		if (m_Fuzz > 0.f)
			reflected = glm::normalize(reflected + m_Fuzz*Math::randomUnitSphere());
		scattered = Math::Ray(rec.position, reflected);
		attenuation = m_Albedo;

		return glm::dot(scattered.direction(), rec.normal) > 0;
	}

private:

	glm::vec3 m_Albedo;
	float m_Fuzz;
};
