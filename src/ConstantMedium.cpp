#include "ConstantMedium.h"
#include <Isotropic.h>
#include <iostream>
#include <Math/Random.h>

ConstantMedium::ConstantMedium(const HitablePtr& boundary, float density, const TexturePtr& ph)
	: m_Boundary(boundary)
	, m_Density(density)
{
	m_PhaseFunction = std::make_shared<Isotropic>(ph);
}

bool ConstantMedium::hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const
{
	const bool bDebug = false;
	HitRecord rec1, rec2;

	if (!m_Boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
		return false;
	if (!m_Boundary->hit(r, rec1.t + 0.001f, FLT_MAX, rec2))
		return false;
	if (bDebug)
		std::cerr << "\nt0 t1 " << rec1.t << " " << rec2.t << "\n";

	if (rec1.t < t0)
		rec1.t = t0;
	if (rec2.t > t1)
		rec2.t = t1;
	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	float distance_inside_boundary = (rec2.t - rec1.t);
	float hit_distance = - (1 / m_Density) * glm::log(Math::BaseRandom());

	if (hit_distance >= distance_inside_boundary)
		return false;

	if (bDebug)
		std::cerr << "hit_distance = " << hit_distance << "\n";

	rec.t = rec1.t + hit_distance;

	if (bDebug)
		std::cerr << "rec.t = " << rec.t << "\n";

	rec.position = r.position(rec.t);

	if (bDebug)
		std::cerr << "rec.position = { " << rec.position.x << " " << rec.position.y << " " << rec.position.z << " } \n";

	rec.normal = glm::vec3(1, 0, 0); // arbitary
	rec.material = m_PhaseFunction;

	return true;
}

bool ConstantMedium::boundingBox(float t0, float t1, Math::AABB& box) const
{
	assert(m_Boundary != nullptr);
	return m_Boundary->boundingBox(t0, t1, box);
}
