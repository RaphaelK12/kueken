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
	const char* TEXTURE_DIFFUSE = "../data/k�ken256.tga";
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(0);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

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

	std::unique_ptr<kueken::renderer> Renderer(nullptr);
	
	kueken::blend::name Blend(kueken::blend::name::null());
	kueken::blit::name Blit(kueken::blit::name::null());
	kueken::rasterizer::name RasterizerMultisample(kueken::rasterizer::name::null());
	kueken::rasterizer::name RasterizerSplash(kueken::rasterizer::name::null());
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

	kueken::framebuffer::name RendertargetFramebuffer(kueken::framebuffer::name::null());
	kueken::framebuffer::name RendertargetResolver(kueken::framebuffer::name::null());
	kueken::framebuffer::name RendertargetMultisample(kueken::framebuffer::name::null());

	kueken::texture::name TextureRendertarget(kueken::texture::name::null());
	kueken::texture::name TextureDiffuse(kueken::texture::name::null());

	kueken::program::semantic const SEMANTIC_UNIF_DIFFUSE(0);
	kueken::program::semantic const SEMANTIC_UNIF_MVP(1);
	kueken::program::semantic const SEMANTIC_ATTR_POSITION(0);
	kueken::program::semantic const SEMANTIC_ATTR_TEXCOORD(1);
	kueken::program::semantic const SEMANTIC_FRAG_COLOR(0);
}

void display()
{
	Renderer->bind(kueken::test::TEST, Test);
	Renderer->bind(kueken::blend::BLEND, Blend);

	{
		glm::mat4 Projection = glm::perspective(45.0f, float(Window.Size.x) / float(Window.Size.y), 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		kueken::program::object & Object = Renderer->map(ProgramOffscreen);
		Object.setSampler(SEMANTIC_UNIF_DIFFUSE, 0);
		Object.setUniform(SEMANTIC_UNIF_MVP, MVP);
		Renderer->unmap(ProgramOffscreen);
		glf::checkError("Render 1");

		Renderer->bind(kueken::rasterizer::RASTERIZER, RasterizerMultisample);
		Renderer->bind(kueken::framebuffer::EXEC, RendertargetMultisample);
		Renderer->exec(ClearOffscreen);

		Renderer->bind(0, kueken::program::UNIFIED, ProgramOffscreen);
		Renderer->bind(0, kueken::sampler::SAMPLER, SamplerDiffuse);
		Renderer->bind(0, kueken::texture::TEXTURE2D, TextureDiffuse);

		Renderer->bind(0, kueken::buffer::ELEMENT, ElementBuffer);
		Renderer->bind(1, kueken::buffer::ARRAY, ArrayBuffer);
		Renderer->bind(0, kueken::layout::VERTEX, Layout);
		glf::checkError("Render 2");

		Renderer->exec(Draw);
		glf::checkError("Render 3");

		Renderer->bind(kueken::framebuffer::READ, RendertargetMultisample);
		Renderer->bind(kueken::framebuffer::DRAW, RendertargetResolver);
		Renderer->exec(Blit);
		glf::checkError("Render 4");
	}

	{
		kueken::program::object & Object = Renderer->map(ProgramOutput);
		Object.setSampler(SEMANTIC_UNIF_DIFFUSE, 0);
		Renderer->unmap(ProgramOutput);

		glf::checkError("Render 6");
		Renderer->bind(kueken::rasterizer::RASTERIZER, RasterizerOutput);
		Renderer->bind(kueken::framebuffer::EXEC, FramebufferOutput);
		Renderer->exec(Clear);

		glf::checkError("Render 9");
		Renderer->bind(0, kueken::program::UNIFIED, ProgramOutput);
		Renderer->bind(0, kueken::sampler::SAMPLER, SamplerOutput);
		Renderer->bind(0, kueken::texture::RECT, TextureColorbuffer);

		Renderer->bind(0, kueken::buffer::ELEMENT, ElementBuffer);
		Renderer->bind(1, kueken::buffer::ARRAY, ArrayBuffer);
		Renderer->bind(0, kueken::layout::VERTEX, Layout);

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

	Creator.setColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
	ClearOffscreen = Renderer->create(Creator);

	Creator.setColor(glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
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

	{
		Creator.setInstances(1);
		Draw = Renderer->create(Creator);
	}

	return glf::checkError("initDraw");
}

bool initTexture2D()
{
	{
		kueken::texture::creator<kueken::texture::IMAGE> Creator(*Renderer);
		Creator.setTarget(kueken::texture::TEXTURE2D);
		Creator.setFormat(kueken::texture::RGBA8);
		Creator.setImage(0, glm::uvec3(Window.Size, glm::uint(1)), 0);
		TextureRendertarget = Renderer->create(Creator);
	}

	{
		gli::texture2D Texture = gli::load(TEXTURE_DIFFUSE);

		kueken::texture::creator<kueken::texture::IMAGE> Creator(*Renderer);
		Creator.setFormat(kueken::texture::RGB8);
		Creator.setTarget(kueken::texture::TEXTURE2D);
		for(kueken::texture::level Level = 0; Level < Texture.levels(); ++Level)
		{
			Creator.setImage(
				Level, 
				glm::uvec3(Texture[Level].dimensions(), 1),
				Texture[Level].data());
		}

		TextureDiffuse = Renderer->create(Creator);
		kueken::texture::object & TextureDiffuseObject = Renderer->map(TextureDiffuse);
		TextureDiffuseObject.generateMipmaps();
		Renderer->unmap(TextureDiffuse);
	}


	return glf::checkError("initTexture2D");
}

bool initTest()
{
	kueken::test::creator Creator(*Renderer);
	Creator.setDepthEnable(true);
	Creator.setDepthFunc(kueken::test::LEQUAL);
	Test = Renderer->create(Creator);

	return glf::checkError("initTest");
}

bool initRasterizer()
{
	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator(*Renderer);
		Creator.setViewport(glm::ivec4(0, 0, Window.Size));
		Creator.setScissor(false, glm::ivec4(Window.Size >> 2, Window.Size >> 1));
		Creator.setMultisample(true);
		RasterizerMultisample = Renderer->create(Creator);
	}

	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator(*Renderer);
		Creator.setViewport(glm::ivec4(0, 0, Window.Size));
		Creator.setScissor(false, glm::ivec4(Window.Size >> 2, Window.Size >> 1));
		Creator.setMultisample(false);
		RasterizerSplash = Renderer->create(Creator);
	}

	return glf::checkError("initRasterizer");
}

bool initSampler()
{
	{
		kueken::sampler::creator Creator(*Renderer);
		Creator.setFilter(kueken::sampler::TRILINEAR);
		Creator.setWrap(kueken::sampler::REPEAT, kueken::sampler::REPEAT, kueken::sampler::REPEAT);
		Creator.setAnisotropy(16.f);
		SamplerDiffuse = Renderer->create(Creator);
	}

	{
		kueken::sampler::creator Creator(*Renderer);
		Creator.setFilter(kueken::sampler::NEAREST);
		Creator.setWrap(kueken::sampler::REPEAT, kueken::sampler::REPEAT, kueken::sampler::REPEAT);
		Creator.setAnisotropy(1.f);
		SamplerSplash = Renderer->create(Creator);
	}

	return glf::checkError("initSampler");
}

bool initProgram()
{
	{
		kueken::program::creator Creator(*Renderer);
		Creator.setVersion(kueken::program::CORE_400);
		Creator.addSource(
			kueken::program::VERTEX, 
			kueken::program::FILE,
			VERT_SHADER_SOURCE0);
		Creator.addSource(
			kueken::program::FRAGMENT, 
			kueken::program::FILE,
			FRAG_SHADER_SOURCE0);
		Creator.addVariable(SEMANTIC_UNIF_DIFFUSE, "Diffuse");
		Creator.addVariable(SEMANTIC_UNIF_MVP, "MVP");
		Creator.addSemantic(SEMANTIC_ATTR_POSITION, "ATTR_POSITION");
		Creator.addSemantic(SEMANTIC_ATTR_TEXCOORD, "ATTR_TEXCOORD");
		Creator.addSemantic(SEMANTIC_FRAG_COLOR, "FRAG_COLOR");
		Creator.build();
		ProgramOffscreen = Renderer->create(Creator);
	}

	{
		kueken::program::creator Creator(*Renderer);
		Creator.setVersion(kueken::program::CORE_400);
		Creator.addSource(
			kueken::program::VERTEX, 
			kueken::program::FILE,
			VERT_SHADER_SOURCE1);
		Creator.addSource(
			kueken::program::FRAGMENT, 
			kueken::program::FILE,
			FRAG_SHADER_SOURCE1);
		Creator.addVariable(SEMANTIC_UNIF_DIFFUSE, "Diffuse");
		Creator.addSemantic(SEMANTIC_ATTR_POSITION, "ATTR_POSITION");
		Creator.addSemantic(SEMANTIC_FRAG_COLOR, "FRAG_COLOR");
		Creator.build();
		ProgramOutput = Renderer->create(Creator);
	}

	return glf::checkError("initProgram");
}

bool initUniformBuffer()
{
	kueken::buffer::creator Creator(*Renderer);
	Creator.setSize(sizeof(glm::mat4) * 2);
	Creator.setData(0);
	Creator.setUsage(kueken::buffer::STREAM_READ);
	UniformBuffer = Renderer->create(Creator);

	return glf::checkError("initUniformBuffer");
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
		Creator.setTexture(kueken::framebuffer::COLOR0, TextureRendertarget, 0);
		RendertargetResolver = Renderer->create(Creator);
	}

	{
		kueken::renderbuffer::creator RenderbufferDepthCreator(*Renderer);
		RenderbufferDepthCreator.setFormat(kueken::renderbuffer::DEPTH24);
		RenderbufferDepthCreator.setSize(glm::uvec2(Window.Size));
		RenderbufferDepthCreator.setSamples(4);
		kueken::renderbuffer::name RenderbufferDepth = Renderer->create(RenderbufferDepthCreator);

		kueken::renderbuffer::creator RenderbufferColor0Creator(*Renderer);
		RenderbufferColor0Creator.setFormat(kueken::renderbuffer::RGBA);
		RenderbufferColor0Creator.setSize(glm::uvec2(Window.Size));
		RenderbufferColor0Creator.setSamples(4);
		kueken::renderbuffer::name RenderbufferColor0 = Renderer->create(RenderbufferColor0Creator);

		kueken::framebuffer::creator<kueken::framebuffer::CUSTOM> RendertargetCreator(*Renderer);
		RendertargetCreator.setRenderbuffer(kueken::framebuffer::DEPTH, RenderbufferDepth);
		RendertargetCreator.setRenderbuffer(kueken::framebuffer::COLOR0, RenderbufferColor0);
		RendertargetMultisample = Renderer->create(RendertargetCreator);
	}

	{
		kueken::framebuffer::creator<kueken::framebuffer::FRAMEBUFFER> Creator(*Renderer);
		RendertargetFramebuffer = Renderer->create(Creator);
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
		Validated = initUniformBuffer();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initArrayBuffer();
	if(Validated)
		Validated = initSampler();
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
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION))
		return 0;
	return 1;
}