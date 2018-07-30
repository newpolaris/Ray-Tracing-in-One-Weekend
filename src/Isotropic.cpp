#include "Isotropic.h"
#include "Math/Random.h"

bool Isotropic::scatter(const Math::Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const
{
	scattered = Math::Ray(rec.position, Math::randomUnitSphere());
	attenuation = m_Albedo->value(rec.u, rec.v, rec.position);
	return true;
}
