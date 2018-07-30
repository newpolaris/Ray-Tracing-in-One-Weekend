#include "BvhNode.h"
#include <algorithm>
#include <Math/Random.h>
#include <iostream>

BvhNode::BvhNode()
{
}

BvhNode::BvhNode(const HitableIter& begin, const HitableIter& end, float t0, float t1)
	: m_Time0(t0)
	, m_Time1(t1)
{
    build(begin, end, m_Time0, m_Time1);
}

BvhNode::BvhNode(const std::vector<HitablePtr>& list, float t0, float t1)
	: m_List(list)
    , m_Time0(t0)
	, m_Time1(t1)
{
    build(m_List.begin(), m_List.end(), m_Time0, m_Time1);
}

BvhNode::BvhNode(std::vector<HitablePtr>&& list, float t0, float t1) noexcept
	: m_List(std::move(list))
    , m_Time0(t0)
	, m_Time1(t1)
{
    build(m_List.begin(), m_List.end(), m_Time0, m_Time1);
}

void BvhNode::build(const HitableIter& begin, const HitableIter& end, float t0, float t1)
{
	int axis = int(3*Math::BaseRandom());
	std::sort(begin, end, 
			[t0, t1, axis](const HitablePtr& a, const HitablePtr& b) 
			{
				assert(a != nullptr);
				assert(b != nullptr);
				Math::AABB left, right;
				auto bA = a->boundingBox(t0, t1, left);
				auto bB = b->boundingBox(t0, t1, right);
				if (!bA || !bB)
					std::cerr << "No bounding box in BvhNode constructor\n" << std::endl;
				return left.min()[axis] < right.min()[axis];
			});

	auto n = std::distance(begin, end);
	assert(n > 0);
	if (n == 1)
	{
		m_Left = m_Right = *begin;
	}
	else if (n == 2)
	{
		m_Left = *begin;
		m_Right = *std::next(begin, 1);
	}
	else
	{
		auto half = std::next(begin, n/2);
		m_Left = std::make_shared<BvhNode>(begin, half, t0, t1);
		m_Right = std::make_shared<BvhNode>(half, end, t0, t1);
	}

	Math::AABB left, right;
	auto bA = m_Left->boundingBox(t0, t1, left);
	auto bB = m_Right->boundingBox(t0, t1, right);
	if (!bA || !bB)
		std::cerr << "No bounding box in BvhNode constructor\n" << std::endl;
	m_BoundingBox = Math::Union(left, right);
}

bool BvhNode::hit(const Math::Ray& r, float tMin, float tMax, HitRecord& rec) const
{
	if (!m_BoundingBox.hit(r, tMin, tMax))
		return false;

	HitRecord left, right;
	auto bLeft = m_Left->hit(r, tMin, tMax, left);
	auto bRight  = m_Right->hit(r, tMin, tMax, right);
	if (!bLeft && !bRight)
		return false;
	if (bLeft && bRight)
		rec = (left.t < right.t) ? left : right;
	else if (bLeft)
		rec = left;
	else
		rec = right;
	return true;
}

bool BvhNode::boundingBox(float t0, float t1, Math::AABB& box) const
{
	box = m_BoundingBox;
	return true;
}
