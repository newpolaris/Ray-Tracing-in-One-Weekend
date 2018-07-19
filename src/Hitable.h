#pragma once

#include <Math/Ray.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <Types.h>

struct HitRecord 
{
	float t;
	glm::vec3 position;
	glm::vec3 normal;
	MaterialPtr material;
};

class Hitable
{
public:
	virtual bool hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
};

typedef std::shared_ptr<Hitable> HitablePtr;
typedef std::vector<HitablePtr> HitableList;

template <typename iterator_type>
bool hit(iterator_type it, iterator_type end, const Math::Ray& r, float tMin, float tMax, HitRecord& rec)
{
	HitRecord record = { 0, };
	bool bHit = false;
	float closest = tMax;
	while (it != end)
	{
		auto ptr = *it++; 
		if (ptr->hit(r, tMin, closest, record))
		{
			bHit = true;
			closest = record.t;
			rec = record;
		}
	}
	return bHit;
}
