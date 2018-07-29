#include <glm/glm.hpp>
#include <Types.h>
#include <Hitable.h>
#include <Math/Ray.h>
#include <HitableSet.h>
#include <Rect.h>
#include <FlipNormal.h>

class Box : public Hitable
{
public:

	Box();
	Box(const glm::vec3& p0, const glm::vec3& p1, const MaterialPtr& ptr);

	virtual bool hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const;

private:

	glm::vec3 m_Min, m_Max;
	HitableSet m_PlaneList;
};

inline Box::Box()
{
}

inline Box::Box(const glm::vec3& p0, const glm::vec3& p1, const MaterialPtr& ptr)
	: m_Min(p0)
	, m_Max(p1)
{
	std::vector<HitablePtr> list;
	list.emplace_back(std::make_shared<RectXY>(m_Min.x, m_Max.x, m_Min.y, m_Max.y, m_Max.z, ptr));
	list.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectXY>(m_Min.x, m_Max.x, m_Min.y, m_Max.y, m_Min.z, ptr)));
	list.emplace_back(std::make_shared<RectXZ>(m_Min.x, m_Max.x, m_Min.z, m_Max.z, m_Max.y, ptr));
	list.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectXZ>(m_Min.x, m_Max.x, m_Min.z, m_Max.z, m_Min.y, ptr)));
	list.emplace_back(std::make_shared<RectYZ>(m_Min.y, m_Max.y, m_Min.z, m_Max.z, m_Max.x, ptr));
	list.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectYZ>(m_Min.y, m_Max.y, m_Min.z, m_Max.z, m_Min.x, ptr)));

	m_PlaneList = std::move(list);
}

inline bool Box::hit(const Math::Ray& r, float t0, float t1, HitRecord& rec) const
{
	return m_PlaneList.hit(r, t0, t1, rec);
}

inline bool Box::boundingBox(float t0, float t1, Math::AABB& box) const
{
	box = Math::AABB(m_Min, m_Max);
	return true;
}
