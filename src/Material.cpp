#include "Material.h"
#include <Texture.h>

bool Metal::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray & scattered) const
{
    glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);
    if (m_Fuzz > 0.f)
        reflected = glm::normalize(reflected + m_Fuzz*Math::randomUnitSphere());
    scattered = Math::Ray(rec.position, reflected, in.time());
    attenuation = m_Albedo->value(0, 0, rec.position);

    return glm::dot(scattered.direction(), rec.normal) > 0;
}

bool Lambertian::scatter(const Math::Ray & in, const HitRecord & rec, glm::vec3 & attenuation, Math::Ray & scattered) const
{
    glm::vec3 target = rec.position + rec.normal + Math::randomUnitSphere();
    glm::vec3 dir = glm::normalize(target - rec.position);

    scattered = Math::Ray(rec.position, dir, in.time());
    attenuation = m_Albedo->value(0.f, 0.f, rec.position);

    return true;
}
