#pragma once

#include <glm/glm.hpp>

namespace Math {

class Bound2
{
public:

    Bound2(glm::ivec2 pt) : m_Min(std::move(pt)), m_Max(std::move(pt))
	{
	}

	Bound2(glm::ivec2 p1, glm::ivec2 p2)
	{
		m_Min = glm::ivec2(glm::min(p1.x, p2.x), glm::min(p1.y, p2.y));
		m_Max = glm::ivec2(glm::max(p1.x, p2.x), glm::max(p1.y, p2.y));
	}

	bool operator==(const Bound2& other) const { return m_Min == other.m_Min && m_Max == other.m_Max; }

	glm::ivec2 m_Min, m_Max;
};

class Bound2Iterator
{
public:

	using value_type = glm::ivec2;
	using difference_type = std::ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_category = std::input_iterator_tag;

	explicit Bound2Iterator(Bound2 bound, glm::ivec2 pt) : m_Position(std::move(pt)), m_Bound(std::move(bound))
	{
	}

	value_type operator*() const { return m_Position; }
	bool operator==(const Bound2Iterator& other) const { return m_Position == other.m_Position && m_Bound == other.m_Bound; }
	bool operator!=(const Bound2Iterator& other) const { return !(*this == other); }

	Bound2Iterator operator++(int)
	{
		Bound2Iterator ret(*this);
		next();
		return ret;
	}

	Bound2Iterator& operator++()
	{
		next();
		return *this;
	}

private:

	void next()
	{
		m_Position.x++;
		if (m_Position.x == m_Bound.m_Max.x)
		{
			m_Position.x = m_Bound.m_Min.x;
			m_Position.y++;
		}
	}

	glm::ivec2 m_Position;
	Bound2 m_Bound;
};

inline Bound2Iterator begin(const Bound2& b)
{
	return Bound2Iterator(b, b.m_Min);
}

inline Bound2Iterator end(const Bound2& b)
{
	// next(last) == next(max-1, may-1) == (minx, maxy)
	glm::ivec2 end(b.m_Min.x, b.m_Max.y);
	// handle degenerate case (zero width etc)
	if (b.m_Min.x >= b.m_Max.x || b.m_Min.y >= b.m_Max.y)
		end = b.m_Min;
	return Bound2Iterator(b, end);
}

}
