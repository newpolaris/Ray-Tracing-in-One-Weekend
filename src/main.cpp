#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <tools/stb_image_write.h>
#include <tools/Parallel.h>

#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <Math/Ray.h>
#include <Math/Random.h>
#include <Math/Perlin.h>
#include "Atmosphere.h"
#include "Sphere.h"
#include "Camera.h"
#include <Material.h>
#include <MovingSphere.h>
#include <HitableSet.h>
#include <BvhNode.h>
#include <ConstantTexture.h>
#include <NoiseTexture.h>
#include <CheckerTexture.h>
#include <ImageTexture.h>
#include <DiffuseLight.h>
#include <Rect.h>
#include <FlipNormal.h>
#include <Box.h>
#include <Translate.h>
#include <Rotate.h>
#include <Pdf.h>
#include <ConstantMedium.h>

namespace 
{
	const int NumSamples = 100;
	const float aperture = 0.0f;
	const int width = 480;
	const int height = 320;
	const float aspect = float(width)/height;

	glm::vec3 lookfrom(278, 278, -800);
	glm::vec3 lookat(278, 278, 0);
	float focusDistance = 10.0f;
}

glm::vec3 de_nan(glm::vec3 c)
{
	return glm::vec3(
			!(c[0] == c[0]) ? 0 : c[0],
            !(c[1] == c[1]) ? 0 : c[1], 
            !(c[2] == c[2]) ? 0 : c[2]); 
}

class ShapeBuilder
{
public:

	ShapeBuilder(const HitablePtr& hitable) : m_Hitable(hitable) {}
	ShapeBuilder flip()
	{
		return ShapeBuilder(std::make_shared<FlipNormal>(m_Hitable));
	}

	ShapeBuilder rotate(const glm::vec3& axis, float angle)
	{
		return ShapeBuilder(std::make_shared<Rotate>(m_Hitable, glm::angleAxis(glm::radians(angle), axis)));
	}

	ShapeBuilder translate(const glm::vec3& t)
	{
		return ShapeBuilder(std::make_shared<Translate>(m_Hitable, t));
	}

	HitablePtr get() { return m_Hitable; }

private:

	HitablePtr m_Hitable;
};

template <typename T, typename ...Args>
static ShapeBuilder BuildShape(Args... args)
{
	return ShapeBuilder(std::make_shared<T>(std::forward<Args>(args)...));
}

glm::vec3 color(const Math::Ray& ray, const HitablePtr& world, const HitablePtr& light_shape, int depth) 
{
	const float RAY_MIN = 1e-3f;
	const float RAY_MAX = 1e8f;

	HitRecord hrec = { 0 };
	if (!world->hit(ray, RAY_MIN, RAY_MAX, hrec)) 
        return glm::vec3(0.f);

    glm::vec3 emitted = hrec.material->emitted(ray, hrec);
    if (depth >= 50)
        return emitted;

	ScatterRecord srec = { 0 };
    if (!hrec.material->scatter(ray, hrec, srec))
        return emitted;

	if (srec.bSpecular)
		return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
	else
	{
		auto plight = std::make_shared<HitablePdf>(light_shape, hrec.position);
		MixturePdf p(plight, srec.pdf_ptr);
		Math::Ray scattered = Math::Ray(hrec.position, p.generate(), ray.time());
		float pdf_value = p.probability(scattered.direction());

		if (pdf_value <= 0.f) return emitted;

		auto source = color(scattered, world, light_shape, depth + 1);
		auto scatteringPdf = hrec.material->scatteringPdf(ray, hrec, scattered);

		return emitted + (srec.attenuation * source * scatteringPdf / pdf_value);
	}
}

HitableList cornellBox()
{
	auto texRed = std::make_shared<ConstantTexture>(glm::vec3(0.65f, 0.05f, 0.05f));
	auto texWhite = std::make_shared<ConstantTexture>(glm::vec3(0.73f));
	auto texGreen = std::make_shared<ConstantTexture>(glm::vec3(0.12f, 0.45f, 0.15f));
	auto texLight = std::make_shared<ConstantTexture>(glm::vec3(15.f));
	auto texAluminum = std::make_shared<ConstantTexture>(glm::vec3(0.8f, 0.85f, 0.88f));
	auto matRed = std::make_shared<Lambertian>(texRed);
	auto matWhite = std::make_shared<Lambertian>(texWhite);
	auto matGreen = std::make_shared<Lambertian>(texGreen);
	auto matLight = std::make_shared<DiffuseLight>(texLight);
	auto matGlass = std::make_shared<Dielectric>(1.5f);

	HitableList world;

	// Light
	world.emplace_back(BuildShape<RectXZ>(213.f, 343.f, 227.f, 332.f, 554.f, matLight).flip().get());

	// Booth
	world.emplace_back(BuildShape<RectYZ>(0.f, 555.f, 0.f, 555.f, 555.f, matGreen).flip().get());
	world.emplace_back(std::make_shared<RectYZ>(0.f, 555.f, 0.f, 555.f, 0.f, matRed));
	world.emplace_back(BuildShape<RectXZ>(0.f, 555.f, 0.f, 555.f, 555.f, matWhite).flip().get());
	world.emplace_back(std::make_shared<RectXZ>(0.f, 555.f, 0.f, 555.f, 0.f, matWhite));
	world.emplace_back(BuildShape<RectXY>(0.f, 555.f, 0.f, 555.f, 555.f, matWhite).flip().get());

	// Box
	world.emplace_back(BuildShape<Box>(glm::vec3(0), glm::vec3(165, 165, 165), matWhite)
			.rotate(glm::vec3(0, 1, 0), 15.f)
			.translate(glm::vec3(265, 0, 295))
			.get());

	// Sphere
	world.emplace_back(BuildShape<Sphere>(glm::vec3(190.f, 90.f, 190.f), 90.f, matGlass).get());

	return world;
}

void render(std::vector<glm::vec4>& image, int width, int height)
{
	Camera camera(lookfrom, lookat, glm::vec3(0, 1, 0), 40.f, aspect, aperture, focusDistance, 0.f, 1.0f);
	HitableList scene = cornellBox();

	auto world = std::make_shared<BvhNode>(scene, 0.f, 1.f);
	const HitablePtr light_shape = std::make_shared<RectXZ>(213.f, 343.f, 227.f, 332.f, 554.f, nullptr);
	const HitablePtr glass_sphere = BuildShape<Sphere>(glm::vec3(190.f, 90.f, 190.f), 90.f, nullptr).get();
	std::vector<HitablePtr> lights;
	lights.emplace_back(BuildShape<RectXZ>(213.f, 343.f, 227.f, 332.f, 554.f, nullptr).get());
	lights.emplace_back(BuildShape<Sphere>(glm::vec3(190.f, 90.f, 190.f), 90.f, nullptr).get());
	auto lightSetPtr = std::make_shared<HitableSet>(lights);
	auto chunksize = height / 4;
    parallel::startup();
    parallel::loop([&](int64_t y)
    {
        for (int x = 0; x < width; x++)
        {
            glm::vec4 c(0.f);
            for (int s = 0; s < NumSamples; s++)
            {
                float u = float(x + Math::BaseRandom()) / width;
                float v = float(y + Math::BaseRandom()) / height;

                auto ray = camera.ray(u, v);
                glm::vec3 cc = color(ray, world, lightSetPtr, 0);
                c += glm::vec4(de_nan(cc), 1.f);
            }
            int index = int(y*width + x);
            image[index] = c / float(NumSamples);
        }
        printf("Process status: height %4d done\n", int(y));
    }, height, chunksize);
    parallel::shutdown();
}

void writeToPNG(std::vector<glm::vec4>& image, int width, int height)
{
	struct rgba { 
		rgba() {}
		rgba(const glm::ivec3& color) : r(color.r), g(color.g), b(color.b), a(0xff) {}
		uint8_t r, g, b, a; 
	};
	std::vector<rgba> temp(width*height*4);
	for (int y = 0; y < height; y++)
	for (int x = 0; x < width; x++)
	{
		auto index = y*width + x;
		glm::vec3 source = glm::vec3(image[index]);
		glm::ivec3 color = glm::pow(source, glm::vec3(1.f/2.2f)) * 255.99f;
		color = glm::min(glm::ivec3(255), color);
		temp[index] = color;
	}
	stbi_flip_vertically_on_write(1);
	stbi_write_png("result.png", width, height, 4, temp.data(), width*4);
}

void writeToPPM(std::vector<glm::vec4>& image, int width, int height)
{
	FILE* pFile = fopen("result.ppm", "wb");
	fprintf(pFile, "P3\n");
	fprintf(pFile, "%d %d\n", width, height);
	fprintf(pFile, "255\n");
	for (int y = height - 1; y >= 0; y--)
	for (int x = 0; x < width; x++)
	{
		glm::vec3 source = glm::vec3(image[y*width + x]);
		glm::ivec3 color = glm::pow(source, glm::vec3(1.f/2.2f)) * 255.99f;
		color = glm::min(glm::ivec3(255), color);
		fprintf(pFile, "%d %d %d\n", color.r, color.g, color.b);
	}
	fclose(pFile);
}

int main()
{
	std::vector<glm::vec4> image(width*height, glm::vec4(0.f));

	render(image, width, height);
	writeToPNG(image, width, height);
	return 0;
}
