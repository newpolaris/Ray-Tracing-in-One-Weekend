#include "DiffuseLight.h"
#include <Texture.h>

DiffuseLight::DiffuseLight(const TexturePtr& emit)
	: m_Emit(emit)
{
}

bool DiffuseLight::scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const
{
	return false;
}

glm::vec3 DiffuseLight::emitted(const Math::Ray& in, const HitRecord& rec) const
{
	if (glm::dot(rec.normal, in.direction()) < 0.f)
		return m_Emit->value(rec.u, rec.v, rec.position);
	return glm::vec3(0.f);
}
