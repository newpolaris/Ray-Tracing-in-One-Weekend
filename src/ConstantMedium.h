#pragma once

#include <Hitable.h>
#include <Material.h>
#include <Math/Ray.h>
#include <Math/AABB.h>

class ConstantMedium : public Hitable
{
public:

	ConstantMedium(const HitablePtr& boundary, float density, const TexturePtr& ph);

	virtual bool hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const;

private:

	float m_Density;
	HitablePtr m_Boundary;
	MaterialPtr m_PhaseFunction;
};
