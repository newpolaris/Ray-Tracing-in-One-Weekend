#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <Types.h>
#include <Math/Ray.h>
#include <Math/AABB.h>

struct HitRecord 
{
	float t, u, v;
	glm::vec3 position;
	glm::vec3 normal;
	MaterialPtr material;
};

class Hitable
{
public:

	virtual ~Hitable();

	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const = 0;
};
