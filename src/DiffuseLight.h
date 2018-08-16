#pragma once

#include <Types.h>
#include <Material.h>

class DiffuseLight : public Material
{
public:

	DiffuseLight(const TexturePtr& emit);

	virtual bool scatter(const Math::Ray& in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override;
	virtual glm::vec3 emitted(const Math::Ray& in, const HitRecord& rec) const override;

private:

	TexturePtr m_Emit;
};
