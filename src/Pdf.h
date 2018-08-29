#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <Math/Frame.h>
#include <Math/Random.h>

class Pdf 
{
public:

	virtual float probability(const glm::vec3& direction) const = 0;
	virtual glm::vec3 generate() const = 0;
};

class CosinePdf : public Pdf
{
public:

	CosinePdf(const glm::vec3& w) : m_Frame(w)
	{
	}

	virtual float probability(const glm::vec3& direction) const override
	{
		const auto pi = glm::pi<float>();
		float cosine = glm::dot(glm::normalize(direction), m_Frame.w());
		return glm::max(0.f, float(cosine/pi));
	}

	virtual glm::vec3 generate() const override
	{
		return m_Frame.local(Math::randomCosineDirection());
	}

private:

	Math::Frame m_Frame;
};

class HitablePdf : public Pdf
{
public:

	HitablePdf(const HitablePtr& hitable, const glm::vec3& origin)
		: m_Hitable(hitable)
		, m_Origin(origin)
	{
	}

	virtual float probability(const glm::vec3& direction) const override
	{
		return m_Hitable->probability(m_Origin, direction);
	}

	virtual glm::vec3 generate() const override
	{
		return m_Hitable->random(m_Origin);
	}

private:

	glm::vec3 m_Origin;
	HitablePtr m_Hitable;
};

class MixturePdf : public Pdf
{
public:

	MixturePdf(const PdfPtr& pdf0, const PdfPtr& pdf1) 
	{
		m_Pdf[0] = pdf0;
		m_Pdf[1] = pdf1;
	}

	virtual float probability(const glm::vec3& direction) const override
	{
		return 0.5f * (m_Pdf[0]->probability(direction) + m_Pdf[1]->probability(direction));
	}

	virtual glm::vec3 generate() const override
	{
		int idx = int(Math::BaseRandom() < 0.5f);
		return m_Pdf[idx]->generate();
	}

private:

	PdfPtr m_Pdf[2];
};
