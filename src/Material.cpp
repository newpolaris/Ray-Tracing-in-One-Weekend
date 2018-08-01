#include "Material.h"
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
