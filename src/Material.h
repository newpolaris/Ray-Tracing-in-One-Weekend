#pragma once

#include <Types.h>
#include <Math/Ray.h>
#include <glm/glm.hpp>

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

	}

private:
};
