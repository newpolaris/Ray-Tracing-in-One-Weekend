#pragma once

#include <Hitable.h>

class Rect : public Hitable
{
public:

private:

};

class RectXY : public Hitable
{
public:

	RectXY();
	RectXY(float x0, float x1, float y0, float y1, float z, const MaterialPtr& material);

	virtual bool hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	MaterialPtr m_Material;
	float m_X0, m_X1, m_Y0, m_Y1, m_Z;
};

class RectXZ : public Hitable
{
public:

	RectXZ();
	RectXZ(float x0, float x1, float z0, float z1, float y, const MaterialPtr& material);

	virtual bool hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;
	virtual float pdf_value(const glm::vec3& origin, const glm::vec3& direction) const override;
	virtual glm::vec3 random(const glm::vec3& origin) const override;

private:

	MaterialPtr m_Material;
	float m_X0, m_X1, m_Z0, m_Z1, m_Y;
};

class RectYZ : public Hitable
{
public:

	RectYZ();
	RectYZ(float y0, float y1, float z0, float z1, float x, const MaterialPtr& material);

	virtual bool hit(const Math::Ray& ray, float t0, float t1, HitRecord& rec) const override;
	virtual bool boundingBox(float t0, float t1, Math::AABB& box) const override;

private:

	MaterialPtr m_Material;
	float m_Y0, m_Y1, m_Z0, m_Z1, m_X;
};
