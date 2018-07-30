#pragma once

#include <Material.h>
#include <Texture.h>
#include <Math/Ray.h>

class Isotropic : public Material
{
public:

	Isotropic(const TexturePtr& albedo) : m_Albedo(albedo) {}

	virtual bool scatter(const Math::Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Math::Ray& scattered) const override;

private:

	TexturePtr m_Albedo;
};
