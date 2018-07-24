#pragma once

#include <glm/glm.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/Random.h>
#include <Math/Scattering.h>
#include <Hitable.h>

struct HitRecord;

class Material
{
public:
	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:

	Lambertian(const TexturePtr& albedo) : m_Albedo(albedo)
	{
	}

    virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override;

private:

	TexturePtr m_Albedo;
};

class Metal : public Material
{
public:

	Metal(const TexturePtr& albedo, float fuzz) : m_Albedo(albedo), m_Fuzz(fuzz)
	{
        assert(albedo != nullptr);
		assert(fuzz <= 1.f);
	}

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override;

private:

	TexturePtr m_Albedo;
	float m_Fuzz;
};

class Dielectric : public Material
{
public:

	Dielectric(float refractIndex) : m_RefractIndex(refractIndex) {}

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override
	{
		float cosine;
		float refractRatio;
		float reflectProbability;
		glm::vec3 refracted;
		glm::vec3 normalOutward;
		glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);

		attenuation = glm::vec3(1.f, 1.f, 1.f);

		if (glm::dot(in.direction(), rec.normal) > 0)
		{
			normalOutward = -rec.normal;
			refractRatio = m_RefractIndex;
			cosine = m_RefractIndex * glm::dot(in.direction(), rec.normal);
		}
		else
		{
			normalOutward = rec.normal;
			refractRatio = 1.f / m_RefractIndex;
			cosine = - glm::dot(in.direction(), rec.normal);
		}

		if (Math::refract(in.direction(), normalOutward, refractRatio, refracted))
			reflectProbability = Math::schlick(cosine, m_RefractIndex);
		else
			reflectProbability = 1.f;

		if (Math::BaseRandom() < reflectProbability) 
			scattered = Math::Ray(rec.position, reflected, in.time());
		else
			scattered = Math::Ray(rec.position, refracted, in.time());
		return true;
	}

private:

	float m_RefractIndex;
};
