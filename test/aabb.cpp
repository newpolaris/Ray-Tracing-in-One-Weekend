#include <gtest/gtest.h>
#include <Math/AABB.h>

TEST(AABB, HitBasic) 
{
	Math::AABB aabb(glm::vec3(1.f), glm::vec3(3.f));
	Math::Ray ray(glm::vec3(0.f), glm::normalize(glm::vec3(1.f)));
	EXPECT_TRUE(aabb.hit(ray, 0.f, 1000.f));
}
