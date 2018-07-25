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

glm::vec3 DiffuseLight::emmitted(float u, float v, const glm::vec3& position) const
{
	return m_Emit->value(u, v, position);
}
