//**********************************
// Kueken sample 06
// 18/10/2010 - 28/10/2010
//**********************************
// Christophe Riccio
// christophe@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>
#include <core/kueken.hpp>

namespace
{
	const char* SAMPLE_NAME = "Kueken sample 06";	
	const char* VERT_SHADER_SOURCE0 = "./data/texture.vert";
	const char* FRAG_SHADER_SOURCE0 = "./data/texture.frag";
	const char* VERT_SHADER_SOURCE1 = "./data/framebuffer.vert";
	const char* FRAG_SHADER_SOURCE1 = "./data/framebuffer.frag";
	const char* TEXTURE_DIFFUSE = "./data/k�ken256dxt5.dds";
	int const SAMPLE_FRAMEBUFFER_X(640);
	int const SAMPLE_FRAMEBUFFER_Y(480);
	int const SAMPLE_OFFSCREEN_X(640);
	int const SAMPLE_OFFSCREEN_Y(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(1);

	glf::window Window(glm::ivec2(SAMPLE_FRAMEBUFFER_X, SAMPLE_FRAMEBUFFER_Y));

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLuint);
	GLuint const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	std::unique_ptr<kueken::renderer> Renderer;
	
	kueken::blend::name Blend(kueken::blend::name::null());
	kueken::blit::name Blit(kueken::blit::name::null());
	kueken::clear::name ClearOffscreen(kueken::clear::name::null());
	kueken::clear::name ClearOutput(kueken::clear::name::null());
	kueken::draw::name Draw(kueken::draw::name::null());
	kueken::program::name ProgramOffscreen(kueken::program::name::null());
	kueken::program::name ProgramOutput(kueken::program::name::null());

	kueken::sampler::name SamplerSplash(kueken::sampler::name::null());
	kueken::sampler::name SamplerDiffuse(kueken::sampler::name::null());

	kueken::test::name Test(kueken::test::name::null());
	kueken::layout::name Layout(kueken::layout::name::null());

	kueken::buffer::name ArrayBuffer(kueken::buffer::name::null());
	kueken::buffer::name ElementBuffer(kueken::buffer::name::null());
	kueken::buffer::name UniformBuffer(kueken::buffer::name::null());

	kueken::framebuffer::name FramebufferOffscreen(kueken::framebuffer::name::null());
	kueken::framebuffer::name FramebufferResolve(kueken::framebuffer::name::null());
	kueken::framebuffer::name FramebufferOutput(kueken::framebuffer::name::null());

	kueken::rasterizer::name RasterizerOffscreen(kueken::rasterizer::name::null());
	kueken::rasterizer::name RasterizerOutput(kueken::rasterizer::name::null());

	kueken::texture::name TextureColorbuffer(kueken::texture::name::null());
	kueken::texture::name TextureDiffuse(kueken::texture::name::null());

	kueken::program::semantic const SEMANTIC_UNIF_DIFFUSE(0);
	kueken::program::semantic const SEMANTIC_UNIF_MVP(1);
	kueken::program::semantic const SEMANTIC_ATTR_POSITION(0);
	kueken::program::semantic const SEMANTIC_ATTR_TEXCOORD(1);
	kueken::program::semantic const SEMANTIC_FRAG_COLOR(0);
}

void display()
{
	Renderer->bind(Test);
	Renderer->bind(Blend);

	{
		glm::mat4 Projection = glm::perspective(45.0f, float(Window.Size.x) / float(Window.Size.y), 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		kueken::program::object & Object = Renderer->map(ProgramOffscreen);
		Object.setSampler(SEMANTIC_UNIF_DIFFUSE, kueken::sampler::SLOT0);
		Object.setUniform(SEMANTIC_UNIF_MVP, MVP);
		Renderer->unmap(ProgramOffscreen);

		Renderer->bind(RasterizerOffscreen);
		Renderer->bind(FramebufferOffscreen, kueken::framebuffer::EXEC);
		Renderer->exec(ClearOffscreen);

		Renderer->bind(ProgramOffscreen, kueken::program::UNIFIED);
		Renderer->bind(SamplerDiffuse, kueken::sampler::SLOT0);
		Renderer->bind(TextureDiffuse, kueken::texture::SLOT0);

		Renderer->bind(ElementBuffer, kueken::buffer::ELEMENT);
		Renderer->bind(ArrayBuffer, kueken::buffer::ARRAY);
		Renderer->bind(Layout);

		Renderer->exec(Draw);

		Renderer->bind(FramebufferOffscreen, kueken::framebuffer::READ);
		Renderer->bind(FramebufferResolve, kueken::framebuffer::DRAW);
		Renderer->exec(Blit);
	}

	{
		kueken::program::object & Object = Renderer->map(ProgramOutput);
		Object.setSampler(SEMANTIC_UNIF_DIFFUSE, kueken::sampler::SLOT0);
		Renderer->unmap(ProgramOutput);

		Renderer->bind(RasterizerOutput);
		Renderer->bind(FramebufferOutput, kueken::framebuffer::EXEC);
		Renderer->exec(ClearOutput);

		Renderer->bind(ProgramOutput, kueken::program::UNIFIED);
		Renderer->bind(SamplerSplash, kueken::sampler::SLOT0);
		Renderer->bind(TextureColorbuffer, kueken::texture::SLOT0);

		Renderer->bind(ElementBuffer, kueken::buffer::ELEMENT);
		Renderer->bind(ArrayBuffer, kueken::buffer::ARRAY);
		Renderer->bind(Layout);

		Renderer->exec(Draw);
	}

	glf::swapBuffers();
	glf::checkError("Render");
}

bool initBlend()
{
	{
		kueken::blend::creator Creator(*Renderer);
		Creator.setBlend(
			kueken::blend::SLOT0, false);
		Creator.setSource(
			kueken::blend::SRC_ALPHA, 
			kueken::blend::SRC_ALPHA);
		Creator.setDestination(
			kueken::blend::ONE_MINUS_SRC_ALPHA, 
			kueken::blend::ONE_MINUS_SRC_ALPHA);
		Creator.setColorMask(
			kueken::blend::SLOT0, 
			glm::bvec4(true, true, true, true));
		Blend = Renderer->create(Creator);
	}

	return glf::checkError("initBlend");
}

bool initBlit()
{
	{
		kueken::blit::creator Creator(*Renderer);
		Creator.setRectSrc(glm::ivec4(0, 0, Window.Size));
		Creator.setRectDst(glm::ivec4(0, 0, Window.Size));
		Creator.setMask(true, false, false);
		Creator.setFilter(kueken::blit::NEAREST);
		Blit = Renderer->create(Creator);
	}

	return glf::checkError("initBlit");
}

bool initClear()
{
	kueken::clear::creator Creator(*Renderer);
	Creator.setDepth(1.0f);

	Creator.setColor(kueken::clear::COLORBUFFER0, glm::vec4(0.6f, 0.8f, 1.0f, 1.0f));
	ClearOffscreen = Renderer->create(Creator);

	Creator.setColor(kueken::clear::COLORBUFFER0, glm::vec4(1.0f, 0.8f, 0.6f, 1.0f));
	ClearOutput = Renderer->create(Creator);

	return glf::checkError("initClear");
}

bool initDraw()
{
	kueken::draw::creator<kueken::draw::ELEMENT> Creator(*Renderer);
	Creator.setPrimitive(kueken::draw::TRIANGLES);
	Creator.setElementFormat(kueken::draw::UINT32);
	Creator.setFirst(0);
	Creator.setCount(ElementCount);
	Creator.setInstances(1);
	Creator.setBaseVertex(0);
	Draw = Renderer->create(Creator);

	return glf::checkError("initDraw");
}

bool initTexture2D()
{
	{
		kueken::texture::creator<kueken::texture::IMAGE> Creator(*Renderer);
		Creator.setTarget(kueken::texture::RECT);
		Creator.setFormat(kueken::texture::RGBA8);
		Creator.setImage(0, glm::uvec3(Window.Size, glm::uint(1)), 0);
		TextureColorbuffer = Renderer->create(Creator);
	}

	{
		gli::texture2D Texture(gli::loadStorageDDS(TEXTURE_DIFFUSE));

		kueken::texture::creator<kueken::texture::IMAGE> Creator(*Renderer);
		Creator.setFormat(kueken::texture::RGBA_DXT5);
		Creator.setTarget(kueken::texture::TEXTURE2D);
		Creator.setLevel(0, static_cast<glm::int32>(Texture.levels()));
		for(kueken::texture::level Level = 0; Level < Texture.levels(); ++Level)
		{
			Creator.setImage(
				Level, 
				Texture[Level].dimensions(), 
				Texture[Level].data());
		}
		TextureDiffuse = Renderer->create(Creator);
	}


	return glf::checkError("initTexture2D");
}

bool initTest()
{
	kueken::test::creator Creator(*Renderer);
	Creator.setDepthEnable(false);
	Creator.setDepthFunc(kueken::test::LEQUAL);
	Test = Renderer->create(Creator);

	return glf::checkError("initTest");
}

bool initRasterizer()
{
	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator(*Renderer);
		Creator.setScissor(kueken::rasterizer::VIEWPORT0, true, glm::ivec4(0, 0, SAMPLE_OFFSCREEN_X, SAMPLE_OFFSCREEN_Y));
		Creator.setViewport(kueken::rasterizer::VIEWPORT0, glm::vec4(0, 0, SAMPLE_OFFSCREEN_X, SAMPLE_OFFSCREEN_Y));
		Creator.setMultisample(true);
		RasterizerOffscreen = Renderer->create(Creator);
	}

	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator(*Renderer);
		Creator.setScissor(kueken::rasterizer::VIEWPORT0, true, glm::ivec4(0, 0, SAMPLE_FRAMEBUFFER_X, SAMPLE_FRAMEBUFFER_Y));
		Creator.setViewport(kueken::rasterizer::VIEWPORT0, glm::vec4(0, 0, SAMPLE_FRAMEBUFFER_X, SAMPLE_FRAMEBUFFER_Y));
		RasterizerOutput = Renderer->create(Creator);
	}

	return glf::checkError("initRasterizer");
}

bool initSampler()
{
	{
		kueken::sampler::creator Creator(*Renderer);
		Creator.setFilter(kueken::sampler::TRILINEAR);
		Creator.setWrap(kueken::sampler::CLAMP_TO_EDGE, kueken::sampler::CLAMP_TO_EDGE, kueken::sampler::CLAMP_TO_EDGE);
		Creator.setAnisotropy(16.f);
		SamplerDiffuse = Renderer->create(Creator);
	}

	{
		kueken::sampler::creator Creator(*Renderer);
		Creator.setFilter(kueken::sampler::NEAREST);
		Creator.setWrap(kueken::sampler::CLAMP_TO_EDGE, kueken::sampler::CLAMP_TO_EDGE, kueken::sampler::CLAMP_TO_EDGE);
		Creator.setAnisotropy(1.f);
		SamplerSplash = Renderer->create(Creator);
	}

	return glf::checkError("initSampler");
}

bool initProgram()
{
	{
		kueken::program::creator Creator(*Renderer);
		Creator.setVersion(kueken::program::CORE_410);
		Creator.addSource(
			kueken::program::VERTEX, 
			kueken::program::FILE,
			VERT_SHADER_SOURCE0);
		Creator.addSource(
			kueken::program::FRAGMENT, 
			kueken::program::FILE,
			FRAG_SHADER_SOURCE0);
		Creator.addSampler(SEMANTIC_UNIF_DIFFUSE, "Diffuse", kueken::program::SAMPLER2D);
		Creator.addUniform(SEMANTIC_UNIF_MVP, "MVP", kueken::program::F32MAT4X4);
		Creator.addSemantic(SEMANTIC_ATTR_POSITION, "ATTR_POSITION");
		Creator.addSemantic(SEMANTIC_ATTR_TEXCOORD, "ATTR_TEXCOORD");
		Creator.addSemantic(SEMANTIC_FRAG_COLOR, "FRAG_COLOR");
		Creator.build();
		ProgramOffscreen = Renderer->create(Creator);
	}

	{
		kueken::program::creator Creator(*Renderer);
		Creator.setVersion(kueken::program::CORE_410);
		Creator.addSource(
			kueken::program::VERTEX, 
			kueken::program::FILE,
			VERT_SHADER_SOURCE1);
		Creator.addSource(
			kueken::program::FRAGMENT, 
			kueken::program::FILE,
			FRAG_SHADER_SOURCE1);
		Creator.addSampler(SEMANTIC_UNIF_DIFFUSE, "Diffuse", kueken::program::SAMPLER2D);
		Creator.addSemantic(SEMANTIC_ATTR_POSITION, "ATTR_POSITION");
		Creator.addSemantic(SEMANTIC_FRAG_COLOR, "FRAG_COLOR");
		Creator.build();
		ProgramOutput = Renderer->create(Creator);
	}

	return glf::checkError("initProgram");
}

bool initLayout()
{
	kueken::layout::creator Creator(*Renderer);
	Creator.setVertexArray(
		0, 
		SEMANTIC_ATTR_POSITION,
		kueken::layout::FLOAT,
		kueken::layout::F32VEC2,
		sizeof(glf::vertex_v2fv2f),
		0, 
		0);
	Creator.setVertexArray(
		0, 
		SEMANTIC_ATTR_TEXCOORD,
		kueken::layout::FLOAT,
		kueken::layout::F32VEC2,
		sizeof(glf::vertex_v2fv2f),
		sizeof(glm::vec2), 
		0);
	Layout = Renderer->create(Creator);

	return glf::checkError("initLayout");
}

bool initArrayBuffer()
{
	{
		kueken::buffer::creator Creator(*Renderer);
		Creator.setSize(VertexSize);
		Creator.setData(VertexData);
		Creator.setUsage(kueken::buffer::STATIC_DRAW);
		ArrayBuffer = Renderer->create(Creator);
	}

	{
		kueken::buffer::creator Creator(*Renderer);
		Creator.setSize(ElementSize);
		Creator.setData(ElementData);
		Creator.setUsage(kueken::buffer::STATIC_DRAW);
		ElementBuffer = Renderer->create(Creator);
	}

	return glf::checkError("initArrayBuffer");
}

bool initFramebuffer()
{
	{
		kueken::framebuffer::creator<kueken::framebuffer::CUSTOM> Creator(*Renderer);
		Creator.setTexture(kueken::framebuffer::COLOR0, TextureColorbuffer, 0);
		FramebufferResolve = Renderer->create(Creator);
	}

	{
		kueken::renderbuffer::creator RenderbufferColor0Creator(*Renderer);
		RenderbufferColor0Creator.setFormat(kueken::renderbuffer::RGBA);
		RenderbufferColor0Creator.setSize(glm::uvec2(Window.Size));
		RenderbufferColor0Creator.setSamples(4);
		kueken::renderbuffer::name RenderbufferColor0 = Renderer->create(RenderbufferColor0Creator);

		kueken::framebuffer::creator<kueken::framebuffer::CUSTOM> RendertargetCreator(*Renderer);
		RendertargetCreator.setRenderbuffer(kueken::framebuffer::COLOR0, RenderbufferColor0);
		FramebufferOffscreen = Renderer->create(RendertargetCreator);
	}

	{
		kueken::framebuffer::creator<kueken::framebuffer::FRAMEBUFFER> Creator(*Renderer);
		FramebufferOutput = Renderer->create(Creator);
	}

	return glf::checkError("initRendertarget");
}

bool begin()
{
	// Check the OpenGL version
	GLint MajorVersion = 0;
	GLint MinorVersion = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);
	bool Validated = glf::version(MajorVersion, MinorVersion) >= glf::version(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);

	Renderer.reset(new kueken::renderer);

	if(Validated)
		Validated = initBlend();
	if(Validated)
		Validated = initBlit();
	if(Validated)
		Validated = initClear();
	if(Validated)
		Validated = initRasterizer();
	if(Validated)
		Validated = initTexture2D();
	if(Validated)
		Validated = initFramebuffer();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initSampler();
	if(Validated)
		Validated = initLayout();
	if(Validated)
		Validated = initTest();
	if(Validated)
		Validated = initDraw();

	Validated = Validated || glf::checkError("Begin");
	return Validated;
}

bool end()
{
	Renderer.reset();

	return glf::checkError("End");
}

int main(int argc, char* argv[])
{
	if(glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_FRAMEBUFFER_X, ::SAMPLE_FRAMEBUFFER_Y), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION))
		return 0;
	return 1;
}
