#include <GL/glew.h>
#include <glfw3.h>

// GLM for matrix transformation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <tools/gltools.hpp>
#include <tools/Profile.h>
#include <tools/imgui.h>
#include <tools/TCamera.h>

#include <GLType/GraphicsDevice.h>
#include <GLType/GraphicsData.h>
#include <GLType/OGLDevice.h>
#include <GLType/ProgramShader.h>
#include <GLType/GraphicsFramebuffer.h>

#include <GLType/OGLTexture.h>
#include <GLType/OGLCoreTexture.h>
#include <GLType/OGLCoreFramebuffer.h>

#include <GraphicsTypes.h>

#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <GameCore.h>
#include <Math/Ray.h>
#include <Math/Random.h>
#include <Math/Perlin.h>
#include "Atmosphere.h"
#include "Sphere.h"
#include "Camera.h"
#include <Material.h>
#include <Mesh.h>
#include <BasicMesh.h>
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
#include <ConstantMedium.h>

enum ProfilerType { ProfilerTypeRender = 0 };

namespace 
{
    float s_CpuTick = 0.f;
    float s_GpuTick = 0.f;
}

struct SceneSettings
{
    bool bCPU = true;
    bool bProfile = true;
    bool bUiChanged = false;
    bool bResized = false;
    bool bUpdated = true;
	bool bChapman = true;
    float angle = 76.f;
    float intensity = 20.f;
    const int numSamples = 4;
};

static float Halton(int index, float base)
{
    float result = 0.0f;
    float f = 1.0f/base;
    float i = float(index);
    for (;;)
    {
        if (i <= 0.0f)
            break;

        result += f*fmodf(i, base);
        i = floorf(i/base);
        f = f/base;
    }
    return result;
}


static std::vector<glm::vec2> Halton2D(int size, int offset)
{
    std::vector<glm::vec2> s(size);
    for (int i = 0; i < size; i++)
    {
        s[i][0] = Halton(i + offset, 2.0f);
        s[i][1] = Halton(i + offset, 3.0f);
    }
    return s;
}

static std::vector<glm::vec4> Halton4D(int size, int offset)
{
    std::vector<glm::vec4> s(size);
    for (int i = 0; i < size; i++)
    {
        s[i][0] = Halton(i + offset, 2.0f);
        s[i][1] = Halton(i + offset, 3.0f);
        s[i][2] = Halton(i + offset, 5.0f);
        s[i][3] = Halton(i + offset, 7.0f);
    }
    return s;
}

glm::mat4 jitterProjMatrix(const glm::mat4& proj, int sampleCount, float jitterAASigma, float width, float height)
{
    // Per-frame jitter to camera for AA
    const int frameNum = sampleCount + 1; // Add 1 since otherwise first sample is an outlier

    float u1 = Halton(frameNum, 2.0f);
    float u2 = Halton(frameNum, 3.0f);

    // Gaussian sample
    float phi = 2.0f*glm::pi<float>()*u2;
    float r = jitterAASigma*sqrtf(-2.0f*log(std::max(u1, 1e-7f)));
    float x = r*cos(phi);
    float y = r*sin(phi);

    glm::mat4 ret = proj;
    ret[0].w += x*2.0f/width;
    ret[1].w += y*2.0f/height;

    return ret;
}

glm::vec3 color(const Math::Ray& ray, const HitablePtr& world, int depth) 
{
	const float RAY_MIN = 1e-3f;
	const float RAY_MAX = 1e8f;

	HitRecord rec = { 0 };
	if (world->hit(ray, RAY_MIN, RAY_MAX, rec)) 
	{
		float pdf;
		glm::vec3 albedo;
		Math::Ray scattered;
		glm::vec3 emmitted = rec.material->emmitted(rec.u, rec.v, rec.position);
		if (depth < 50 && rec.material->scatter(ray, rec, albedo, scattered, pdf))
		{
#if 1
			glm::vec3 on_light = glm::vec3(
					213.f + Math::BaseRandom()*(343 - 213),
					554.f,
					227.f + Math::BaseRandom()*(332 - 227));
			glm::vec3 to_light = on_light - rec.position;
			float distance_squared = glm::dot(to_light, to_light);
			glm::vec3 to_light_norm = glm::normalize(to_light);
			// if (glm::dot(to_light_norm, rec.normal) < 0.f) return emmitted;
			float light_area = (343-213)*(332-227);
			float light_cosine = glm::abs(to_light_norm.y);
			if (light_cosine < 0.00001f)
				return emmitted;
			pdf = distance_squared / (light_cosine * light_area);
			scattered = Math::Ray(rec.position, to_light_norm, ray.time());
#endif

			if (pdf <= 0.f) return emmitted;
			auto source = color(scattered, world, depth + 1);
			auto scatteringPdf = rec.material->scatteringPdf(ray, rec, scattered);

			return emmitted + (albedo * source * scatteringPdf / pdf);
		}
		return emmitted;
	}
	return glm::vec3(0.f);
}

HitableList perlinSpheres()
{
	auto texLight = std::make_shared<ConstantTexture>(glm::vec3(4.f));
    auto texPerlinNoise = std::make_shared<NoiseTexture>();
	auto matLight = std::make_shared<DiffuseLight>(texLight);
	auto matPerlinNoise = std::make_shared<Lambertian>(texPerlinNoise);
	auto texEarth = std::make_shared<ImageTexture>("resources/Earth.jpg");
	auto matEarth = std::make_shared<Lambertian>(texEarth);

	HitableList world;
	world.emplace_back(std::make_shared<Sphere>(glm::vec3(0, -1000, 0), 1000.f, matPerlinNoise));
	world.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 2, 0), 2.0f, matEarth));
	world.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 7, 0), 2.0f, matLight));
	world.emplace_back(std::make_shared<RectXY>(3.f, 5.f, 1.f, 3.f, -2.f, matLight));
	world.emplace_back(std::make_shared<RectXY>(3.f, 5.f, 1.f, 3.f, 5.f, matLight));

	return world;
}

HitableList cornellBox()
{
	auto texRed = std::make_shared<ConstantTexture>(glm::vec3(0.65f, 0.05f, 0.05f));
	auto texWhite = std::make_shared<ConstantTexture>(glm::vec3(0.73f));
	auto texGreen = std::make_shared<ConstantTexture>(glm::vec3(0.12f, 0.45f, 0.15f));
	auto texLight = std::make_shared<ConstantTexture>(glm::vec3(15.f));
	auto matRed = std::make_shared<Lambertian>(texRed);
	auto matWhite = std::make_shared<Lambertian>(texWhite);
	auto matGreen = std::make_shared<Lambertian>(texGreen);
	auto matLight = std::make_shared<DiffuseLight>(texLight);

	HitableList world;

	// Light
	world.emplace_back(std::make_shared<RectXZ>(213.f, 343.f, 227.f, 332.f, 554.f, matLight));

	// Booth
	world.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectYZ>(0.f, 555.f, 0.f, 555.f, 555.f, matGreen)));
	world.emplace_back(std::make_shared<RectYZ>(0.f, 555.f, 0.f, 555.f, 0.f, matRed));
	world.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectXZ>(0.f, 555.f, 0.f, 555.f, 555.f, matWhite)));
	world.emplace_back(std::make_shared<RectXZ>(0.f, 555.f, 0.f, 555.f, 0.f, matWhite));
	world.emplace_back(std::make_shared<FlipNormal>(std::make_shared<RectXY>(0.f, 555.f, 0.f, 555.f, 555.f, matWhite)));

	// Box
	world.emplace_back(
			std::make_shared<Translate>(
				std::make_shared<RotateY>(
					std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 165, 165), matWhite),
					-18.f),
				glm::vec3(130, 0, 65)));

	world.emplace_back(
			std::make_shared<Translate>(
				std::make_shared<RotateY>(
					std::make_shared<Box>(glm::vec3(0), glm::vec3(165, 330, 165), matWhite),
					15.f),
				glm::vec3(265, 0, 295)));

	return world;
}

HitableList finalScene()
{
	const int nb = 20;

    auto matWhite = std::make_shared<Lambertian>(std::make_shared<ConstantTexture>(glm::vec3(0.73f)));
    auto matGround = std::make_shared<Lambertian>(std::make_shared<ConstantTexture>(glm::vec3(0.48, 0.83, 0.53)));

    HitableList ground; 
    for (int i = 0; i < nb; i++)
    for (int j = 0; j < nb; j++)
    {
        float w = 100;
        float x0 = -1000 + i*w;
        float z0 = -1000 + j*w;
        float y0 = 0;
        float x1 = x0 + w;
        float y1 = 100*(Math::BaseRandom() + 0.01f);
        float z1 = z0 + w;
        ground.emplace_back(std::make_shared<Box>(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1), matGround));
    }
    auto bvhGround = std::make_shared<BvhNode>(ground, 0.f, 1.f);

	auto texLight = std::make_shared<ConstantTexture>(glm::vec3(7.f));
	auto matLight = std::make_shared<DiffuseLight>(texLight);

	HitableList list;
    list.push_back(bvhGround);
	list.emplace_back(std::make_shared<RectXZ>(123.f, 423.f, 147.f, 412.f, 554.f, matLight));
	glm::vec3 center(400, 400, 200);
	list.emplace_back(std::make_shared<MovingSphere>(center, center + glm::vec3(30, 0, 0), 0.f, 1.f, 50.f, std::make_shared<Lambertian>(std::make_shared<ConstantTexture>(glm::vec3(0.7, 0.3, 0.1)))));
	list.emplace_back(std::make_shared<Sphere>(glm::vec3(260, 150, 45), 50.f, std::make_shared<Dielectric>(1.5f)));
	list.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 150, 145), 50.f, std::make_shared<Metal>(std::make_shared<ConstantTexture>(glm::vec3(0.8, 0.8, 0.9)), 10.0f)));
	auto smokeball = std::make_shared<Sphere>(glm::vec3(360, 150, 145), 70.f, std::make_shared<Dielectric>(1.5f));
	list.emplace_back(smokeball);
	list.emplace_back(std::make_shared<ConstantMedium>(smokeball, 0.02f, std::make_shared<ConstantTexture>(glm::vec3(0.2f, 0.4f, 0.9f))));
	// global fog
	list.emplace_back(std::make_shared<ConstantMedium>(
				std::make_shared<Sphere>(glm::vec3(0.f), 5000.f, std::make_shared<Dielectric>(1.5f)),
				0.0001f,
				std::make_shared<ConstantTexture>(glm::vec3(1.f))));

	auto texEarth = std::make_shared<ImageTexture>("resources/Earth.jpg");
	auto matEarth = std::make_shared<Lambertian>(texEarth);
	list.emplace_back(std::make_shared<Sphere>(glm::vec3(400, 200, 400), 100.f, matEarth));
    auto texPerlinNoise = std::make_shared<NoiseTexture>();
	auto matPerlinNoise = std::make_shared<Lambertian>(texPerlinNoise);
	list.emplace_back(std::make_shared<Sphere>(glm::vec3(220, 280, 300), 80.f, matPerlinNoise));

	return list;
}

#define SCENE_PERLINE 0
#define SCENE_CORNELL 1
#define SCENE_FINAL 2

#define SCENE_CODE SCENE_CORNELL

void test(std::vector<glm::vec4>& image, int width, int height)
{
	const int NumSamples = 10;
	const float aperture = 0.0f;
	const float aspect = float(width)/height;

#if SCENE_CODE == SCENE_PERLINE
	auto lookfrom = glm::vec3(13, 2, 3);
	auto lookat = glm::vec3(0, 2, 0);
	auto focusDistance = glm::length(lookfrom - lookat);
	Camera camera(lookfrom, lookat, glm::vec3(0, 1, 0), 40.f, aspect, aperture, focusDistance, 0.f, 1.0f);
	HitableList scene = perlinSpheres();
#elif SCENE_CODE == SCENE_CORNELL
	auto lookfrom = glm::vec3(278, 278, -800);
	auto lookat = glm::vec3(278, 278, 0);
	auto focusDistance = 10.0f;
	Camera camera(lookfrom, lookat, glm::vec3(0, 1, 0), 40.f, aspect, aperture, focusDistance, 0.f, 1.0f);
	HitableList scene = cornellBox();
#else
	auto lookfrom = glm::vec3(78, 328, -800);
	auto lookat = glm::vec3(278, 278, 0);
	auto focusDistance = 10.0f;
	Camera camera(lookfrom, lookat, glm::vec3(0, 1, 0), 30.f, aspect, aperture, focusDistance, 0.f, 1.0f);
	HitableList scene = finalScene();
#endif
	auto world = std::make_shared<BvhNode>(scene, 0.f, 1.f);

	#pragma omp parallel for num_threads(3)
	for (int y = height - 1; y >= 0; y--)
	{
		for (int x = width - 1; x >= 0; x--)
		{
			glm::vec4 c(0.f);
			for (int s = 0; s < NumSamples; s++)
			{
				float u = float(x + Math::BaseRandom()) / width;
				float v = float(y + Math::BaseRandom()) / height;

				auto ray = camera.ray(u, v);
				c += glm::vec4(color(ray, world, 0), 1.f);
			}
			image[y*width + x] = c / float(NumSamples);
		}
		printf("Process status: height %4d done\n", y);
	}
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

class RayTracer final : public gamecore::IGameApp
{
public:
	RayTracer() noexcept;
	virtual ~RayTracer() noexcept;

	virtual void startup() noexcept override;
	virtual void closeup() noexcept override;
	virtual void update() noexcept override;
    virtual void updateHUD() noexcept override;
	virtual void render() noexcept override;

	virtual void keyboardCallback(uint32_t c, bool bPressed) noexcept override;
	virtual void framesizeCallback(int32_t width, int32_t height) noexcept override;
	virtual void motionCallback(float xpos, float ypos, bool bPressed) noexcept override;
	virtual void mouseCallback(float xpos, float ypos, bool bPressed) noexcept override;

	GraphicsDevicePtr createDevice(const GraphicsDeviceDesc& desc) noexcept;

private:

    std::vector<glm::vec2> m_Samples;
    SceneSettings m_Settings;
	TCamera m_Camera;
    FullscreenTriangleMesh m_ScreenTraingle;
    ProgramShader m_SkyShader;
    ProgramShader m_BlitShader;
    GraphicsTexturePtr m_SkyColorTex;
    GraphicsTexturePtr m_ScreenColorTex;
    GraphicsFramebufferPtr m_ColorRenderTarget;
    GraphicsDevicePtr m_Device;
};

CREATE_APPLICATION(RayTracer);

RayTracer::RayTracer() noexcept
{
}

RayTracer::~RayTracer() noexcept
{
}

void RayTracer::startup() noexcept
{
	profiler::initialize();
	perlin::initialize();

	m_Camera.setViewParams(glm::vec3(2.0f, 5.0f, 15.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	m_Camera.setMoveCoefficient(0.35f);

	GraphicsDeviceDesc deviceDesc;
#if __APPLE__
	deviceDesc.setDeviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGL);
#else
	deviceDesc.setDeviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore);
#endif
	m_Device = createDevice(deviceDesc);
	assert(m_Device);

	m_SkyShader.setDevice(m_Device);
	m_SkyShader.initialize();
	m_SkyShader.addShader(GL_VERTEX_SHADER, "Scattering.Vertex");
	m_SkyShader.addShader(GL_FRAGMENT_SHADER, "Scattering.Fragment");
	m_SkyShader.link();

	m_BlitShader.setDevice(m_Device);
	m_BlitShader.initialize();
	m_BlitShader.addShader(GL_VERTEX_SHADER, "BlitTexture.Vertex");
	m_BlitShader.addShader(GL_FRAGMENT_SHADER, "BlitTexture.Fragment");
	m_BlitShader.link();

    m_ScreenTraingle.create();

    m_Samples = Halton2D(m_Settings.numSamples, 0);
}

void RayTracer::closeup() noexcept
{
    m_ScreenTraingle.destroy();
	profiler::shutdown();
}

void RayTracer::update() noexcept
{
    bool bCameraUpdated = m_Camera.update();

    static int32_t preWidth = 0;
    static int32_t preHeight = 0;

    int32_t width = getFrameWidth();
    int32_t height = getFrameHeight();
    bool bResized = false;
    if (preWidth != width || preHeight != height)
    {
        preWidth = width, preHeight = height;
        bResized = true;
    }
    m_Settings.bUpdated = (m_Settings.bUiChanged || bCameraUpdated || bResized);
    if (m_Settings.bUpdated && m_Settings.bCPU)
    {
#if 0
		width = 200;
		height = 100;
#endif
        const float angle = glm::radians(m_Settings.angle);
        std::vector<glm::vec4> image(width*height, glm::vec4(0.f));
        glm::vec3 sunDir = glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle));

		profiler::start(ProfilerTypeRender);

		test(image, width, height);
		writeToPPM(image, width, height);

		profiler::stop(ProfilerTypeRender);
		profiler::tick(ProfilerTypeRender, s_CpuTick, s_GpuTick);

        GraphicsTextureDesc colorDesc;
        colorDesc.setWidth(width);
        colorDesc.setHeight(height);
        colorDesc.setFormat(gli::FORMAT_RGBA32_SFLOAT_PACK32);
        colorDesc.setStream((uint8_t*)image.data());
        colorDesc.setStreamSize(width*height*sizeof(glm::vec4));
        m_SkyColorTex = m_Device->createTexture(colorDesc);
    }
}

void RayTracer::updateHUD() noexcept
{
    bool bUpdated = false;
    float width = (float)getWindowWidth(), height = (float)getWindowHeight();

    ImGui::SetNextWindowPos(
        ImVec2(width - width / 4.f - 10.f, 10.f),
        ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Settings",
        NULL,
        ImVec2(width / 4.0f, height - 20.0f),
        ImGuiWindowFlags_AlwaysAutoResize);
    bUpdated |= ImGui::Checkbox("Mode CPU", &m_Settings.bCPU);
    bUpdated |= ImGui::Checkbox("Always redraw", &m_Settings.bProfile);
	bUpdated |= ImGui::Checkbox("Use chapman approximation", &m_Settings.bChapman);
    bUpdated |= ImGui::SliderFloat("Sun Angle", &m_Settings.angle, 0.f, 120.f);
    bUpdated |= ImGui::SliderFloat("Sun Intensity", &m_Settings.intensity, 10.f, 50.f);
    ImGui::Text("CPU %s: %10.5f ms\n", "main", s_CpuTick);
    ImGui::Text("GPU %s: %10.5f ms\n", "main", s_GpuTick);
    ImGui::PushItemWidth(180.0f);
    ImGui::Indent();
    ImGui::Unindent();
    ImGui::End();

    m_Settings.bUiChanged = bUpdated;
}

void RayTracer::render() noexcept
{
	gamecore::closeApplication();
	return;

    bool bUpdate = m_Settings.bProfile || m_Settings.bUpdated;

    if (!m_Settings.bCPU && bUpdate)
    {
        auto& desc = m_ScreenColorTex->getGraphicsTextureDesc();
        m_Device->setFramebuffer(m_ColorRenderTarget);
        GLenum clearFlag = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
        glViewport(0, 0, desc.getWidth(), desc.getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);
        glClear(clearFlag);

        glDisable(GL_DEPTH_TEST);
        float angle = glm::radians(m_Settings.angle);
        float halfFov = m_Camera.getFov()/2.f;
		glm::vec2 resolution(desc.getWidth(), desc.getHeight());
        glm::vec3 sunDir = glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle));
        m_SkyShader.bind();
        m_SkyShader.setUniform("uChapman", m_Settings.bChapman);
        m_SkyShader.setUniform("uEarthRadius", 6360e3f);
        m_SkyShader.setUniform("uAtmosphereRadius", 6420e3f);
		m_SkyShader.setUniform("uInvResolution", 1.f/resolution);
        m_SkyShader.setUniform("uEarthCenter", glm::vec3(0.f));
        m_SkyShader.setUniform("uSunDir", sunDir);
        m_SkyShader.setUniform("uAspect", m_Camera.getAspect());
        m_SkyShader.setUniform("uAngle", glm::tan(glm::radians(halfFov)));
        m_SkyShader.setUniform("uSamples", m_Samples.data(), 4);
        m_SkyShader.setUniform("uSunIntensity", glm::vec3(m_Settings.intensity));
        m_ScreenTraingle.draw();
        glEnable(GL_DEPTH_TEST);
    }
    // Tone mapping
    {
        GraphicsTexturePtr target = m_ScreenColorTex;
        if (m_Settings.bCPU && m_SkyColorTex) 
            target = m_SkyColorTex;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, getFrameWidth(), getFrameHeight());

        glDisable(GL_DEPTH_TEST);
        m_BlitShader.bind();
        m_BlitShader.bindTexture("uTexSource", target, 0);
        m_ScreenTraingle.draw();
        glEnable(GL_DEPTH_TEST);
    }
}

void RayTracer::keyboardCallback(uint32_t key, bool isPressed) noexcept
{
	switch (key)
	{
	case GLFW_KEY_UP:
		m_Camera.keyboardHandler(MOVE_FORWARD, isPressed);
		break;

	case GLFW_KEY_DOWN:
		m_Camera.keyboardHandler(MOVE_BACKWARD, isPressed);
		break;

	case GLFW_KEY_LEFT:
		m_Camera.keyboardHandler(MOVE_LEFT, isPressed);
		break;

	case GLFW_KEY_RIGHT:
		m_Camera.keyboardHandler(MOVE_RIGHT, isPressed);
		break;
	}
}

void RayTracer::framesizeCallback(int32_t width, int32_t height) noexcept
{
	float aspectRatio = (float)width/height;
	m_Camera.setProjectionParams(45.0f, aspectRatio, 0.1f, 100.0f);

    GraphicsTextureDesc colorDesc;
    colorDesc.setWidth(width);
    colorDesc.setHeight(height);
    colorDesc.setFormat(gli::FORMAT_RGBA16_SFLOAT_PACK16);
    m_ScreenColorTex = m_Device->createTexture(colorDesc);

    GraphicsTextureDesc depthDesc;
    depthDesc.setWidth(width);
    depthDesc.setHeight(height);
    depthDesc.setFormat(gli::FORMAT_D24_UNORM_S8_UINT_PACK32);
    auto depthTex = m_Device->createTexture(depthDesc);

    GraphicsFramebufferDesc desc;  
    desc.addComponent(GraphicsAttachmentBinding(m_ScreenColorTex, GL_COLOR_ATTACHMENT0));
    desc.addComponent(GraphicsAttachmentBinding(depthTex, GL_DEPTH_ATTACHMENT));
    
    m_ColorRenderTarget = m_Device->createFramebuffer(desc);;
}

void RayTracer::motionCallback(float xpos, float ypos, bool bPressed) noexcept
{
	const bool mouseOverGui = ImGui::MouseOverArea();
	if (!mouseOverGui && bPressed) m_Camera.motionHandler(int(xpos), int(ypos), false);    
}

void RayTracer::mouseCallback(float xpos, float ypos, bool bPressed) noexcept
{
	const bool mouseOverGui = ImGui::MouseOverArea();
	if (!mouseOverGui && bPressed) m_Camera.motionHandler(int(xpos), int(ypos), true); 
}

GraphicsDevicePtr RayTracer::createDevice(const GraphicsDeviceDesc& desc) noexcept
{
	GraphicsDeviceType deviceType = desc.getDeviceType();

#if __APPLE__
	assert(deviceType != GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore);
#endif

	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore)
    {
        auto device = std::make_shared<OGLDevice>();
        if (device->create(desc))
            return device;
        return nullptr;
    }
    return nullptr;
}
