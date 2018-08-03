#include "Material.h"
#include <glm/gtc/type_ptr.hpp> 
#include <Texture.h>

Material::~Material()
{
}

glm::vec3 Material::emmitted(float u, float v, const glm::vec3& position) const
{
	return glm::vec3(0.f);
}

Lambertian::~Lambertian()
{
}

bool Lambertian::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const
{
    glm::vec3 target = rec.position + rec.normal + Math::randomUnitSphere();
    glm::vec3 dir = glm::normalize(target - rec.position);

    scattered = Math::Ray(rec.position, dir, in.time());
    attenuation = m_Albedo->value(rec.u, rec.v, rec.position);

    return true;
}

bool Lambertian::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& albedo, Math::Ray& scattered, float& pdf) const
{
	const auto pi = glm::pi<float>();
    glm::vec3 target = rec.position + rec.normal + Math::randomUnitSphere();
    glm::vec3 dir = glm::normalize(target - rec.position);

    scattered = Math::Ray(rec.position, dir, in.time());
    albedo = m_Albedo->value(rec.u, rec.v, rec.position);
	pdf = glm::dot(rec.normal, scattered.direction()) / pi;
    pdf = glm::max(0.f, pdf);

    return true;
}

float Lambertian::scatteringPdf(const Math::Ray& in, const HitRecord& rec, const Math::Ray& scattered) const
{
	const auto pi = glm::pi<float>();

	float cosine = glm::dot(rec.normal, scattered.direction());
    cosine = glm::max(cosine, 0.f);
	return cosine / pi;
}

Metal::~Metal()
{
}

bool Metal::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray & scattered) const
{
    glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);
    if (m_Fuzz > 0.f)
        reflected = glm::normalize(reflected + m_Fuzz*Math::randomUnitSphere());
    scattered = Math::Ray(rec.position, reflected, in.time());
    attenuation = m_Albedo->value(rec.u, rec.v, rec.position);

    return glm::dot(scattered.direction(), rec.normal) > 0;
}

Dielectric::~Dielectric()
{
}
