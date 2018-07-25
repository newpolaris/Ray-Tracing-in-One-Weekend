#pragma once

#include <Types.h>
#include <Material.h>

class DiffuseLight : public Material
{
public:

	DiffuseLight(const TexturePtr& emit);

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const;
	virtual glm::vec3 emmitted(float u, float v, const glm::vec3& position) const;

private:

	TexturePtr m_Emit;
};
