#include <HitableSet.h>
#include <algorithm>

HitableSet::HitableSet()
{
}

HitableSet::HitableSet(const std::vector<HitablePtr>& list)
	: m_List(list)
{
}

HitableSet::HitableSet(std::vector<HitablePtr>&& list) noexcept
	: m_List(std::move(list))
{
}

bool HitableSet::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const
{
	HitRecord record = { 0, };
	bool bHit = false;
	float closest = tMax;
	for (auto it : m_List)
	{
		if (it->hit(r, tMin, closest, record))
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
	if (m_List.empty())
		return false;

	Math::AABB bound;
	auto it = m_List.begin();
	auto end = m_List.end();
	auto item = *it++;
	bool bSuccess = item->boundingBox(t0, t1, bound);
	if (!bSuccess)
		return false;

	box = bound;
	while (it != end)
	{
		item = *it++;
		if (!item->boundingBox(t0, t1, bound))
			return false;
		box = Math::Union(box, bound);
	}
	return true;
}
