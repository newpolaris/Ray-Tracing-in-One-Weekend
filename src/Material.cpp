#include "Material.h"
#include <glm/gtc/type_ptr.hpp> 
#include <Texture.h>
#include <Math/Frame.h>
#include <Math/Random.h>
#include <Pdf.h>

Material::~Material()
{
}

glm::vec3 Material::emitted(const Math::Ray& in, const HitRecord& rec) const
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

bool Lambertian::scatter(const Math::Ray& in, const HitRecord& rec, ScatterRecord& srec) const
{
	srec.bSpecular = false;
	srec.attenuation = m_Albedo->value(rec.u, rec.v, rec.position);
	srec.pdf_ptr = std::make_shared<CosinePdf>(rec.normal);
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

bool Metal::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const
{
    glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);
    if (m_Fuzz > 0.f)
        reflected = glm::normalize(reflected + m_Fuzz*Math::randomUnitSphere());
    scattered = Math::Ray(rec.position, reflected, in.time());
    attenuation = m_Albedo->value(rec.u, rec.v, rec.position);

    return glm::dot(scattered.direction(), rec.normal) > 0;
}

bool Metal::scatter(const Math::Ray& in, const HitRecord& rec, ScatterRecord& srec) const
{
    glm::vec3 reflected = glm::reflect(in.direction(), rec.normal);
	glm::vec3 dir = glm::normalize(reflected + m_Fuzz*Math::randomUnitSphere());
    srec.specular_ray = Math::Ray(rec.position, dir, in.time());
    srec.attenuation = m_Albedo->value(rec.u, rec.v, rec.position);
	srec.bSpecular = true;
	srec.pdf_ptr = nullptr;

    return glm::dot(reflected, rec.normal) > 0;
}


Dielectric::~Dielectric()
{
}
