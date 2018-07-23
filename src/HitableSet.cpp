#include <HitableSet.h>
#include <algorithm>

HitableSet::HitableSet()
{
}

HitableSet::HitableSet(const HitableConstIter& begin, const HitableConstIter& end)
	: m_Begin(begin)
	, m_End(end)
{
}

bool HitableSet::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const
{
	HitRecord record = { 0, };
	bool bHit = false;
	float closest = tMax;
	auto it = m_Begin;
	while (it != m_End)
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

bool HitableSet::boundingBox(float t0, float t1, Math::AABB& box) const
{
	if (m_Begin == m_End) 
		return false;

	Math::AABB bound;
	auto it = m_Begin;
	auto item = *it++;
	bool bSuccess = item->boundingBox(t0, t1, bound);
	if (!bSuccess)
		return false;

	box = bound;
	while (it != m_End)
	{
		item = *it++;
		if (!item->boundingBox(t0, t1, bound))
			return false;
		box = Math::Union(box, bound);
	}
	return true;
}
