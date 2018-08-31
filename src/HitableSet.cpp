#include <HitableSet.h>
#include <algorithm>
#include <Math/Random.h>
#include <algorithm>

HitableSet::HitableSet()
{
}

HitableSet::HitableSet(std::vector<HitablePtr> list) noexcept
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

float HitableSet::pdf_value(const glm::vec3& origin, const glm::vec3& direction) const
{
	assert(m_List.size() > 0);
	float sum = 0;
	for (auto& p : m_List)
		sum += p->pdf_value(origin, direction);
	return sum / m_List.size();
}

glm::vec3 HitableSet::random(const glm::vec3& origin) const
{
	size_t n = m_List.size();
	size_t index = size_t(Math::BaseRandom() * n);
	assert(index < n);
	return m_List[index]->random(origin);
}

