//**********************************
// Kueken multisample 02
// 30/01/2009
//**********************************
// Christophe Riccio
// christophe@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include "main.hpp"

namespace
{
	const char* SAMPLE_NAME = "Kueken multisample 02";	
	const char* VERTEX_SHADER_SOURCE = "../data/multisampling.vert";
	const char* FRAGMENT_SHADER_SOURCE = "../data/multisampling.frag";
	const char* TEXTURE_DIFFUSE = "../data/k�ken256.tga";

	GLsizei const VertexCount = 4;
	GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v2fv2f);
	vertex_v2fv2f const VertexData[VertexCount] =
	{
		vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLuint);
	GLuint const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	kueken::renderer * Renderer(nullptr);
	
	kueken::blend::name Blend;
	kueken::blit::name Blit;
	kueken::rasterizer::name RasterizerMultisample;
	kueken::rasterizer::name RasterizerSplash;
	kueken::clear::name Clear;
	kueken::draw::name DrawSplash;
	kueken::draw::name DrawMesh;
	kueken::program::name Program;

	kueken::sampler::name SamplerSplash;
	kueken::sampler::name SamplerDiffuse;

	kueken::buffer::name ArrayBuffer;
	kueken::buffer::name ElementBuffer;
	kueken::test::name Test;
	kueken::layout::name Layout;
	kueken::buffer::name UniformBuffer;

	kueken::rendertarget::name RendertargetFramebuffer;
	kueken::rendertarget::name RendertargetResolver;
	kueken::rendertarget::name RendertargetMultisample;

	kueken::texture::name TextureRendertarget;
	kueken::texture::name TextureDiffuse;

	kueken::program::semantic const SEMANTIC_DIFFUSE(0);
	kueken::program::semantic const SEMANTIC_MVP(1);

	kueken::program::semantic const SEMANTIC_POSITION(0);
	kueken::program::semantic const SEMANTIC_TEXCOORD(4);
}

sample::sample
(
	std::string const & Name, 
	glm::ivec2 const & WindowSize
) :
	base(Name, WindowSize)
{}

sample::~sample()
{}

bool sample::check() const
{
	return 
		GLEW_EXT_framebuffer_object == GL_TRUE && 
		GLEW_EXT_direct_state_access == GL_TRUE && 
		GLEW_EXT_texture_buffer_object == GL_TRUE && 
		GLEW_EXT_texture_swizzle == GL_TRUE;
}

bool sample::begin(glm::ivec2 const & WindowSize)
{
	windowSize = WindowSize;

	Renderer = new kueken::renderer;

	bool Result = true;
	if(Result)
		Result = initBlend();
	if(Result)
		Result = initBlit();
	if(Result)
		Result = initClear();
	if(Result)
		Result = initRasterizer();
	if(Result)
		Result = initRendertarget();
	if(Result)
		Result = initUniformBuffer();
	if(Result)
		Result = initProgram();
	if(Result)
		Result = initArrayBuffer();
	if(Result)
		Result = initAssembler();
	if(Result)
		Result = initSampler();
	if(Result)
		Result = initTexture2D();
	if(Result)
		Result = initTest();
	if(Result)
		Result = initDraw();

	Result = Result || glf::checkError("Begin");
	return Result;
}

bool sample::end()
{
	delete Renderer;
	Renderer = 0;

	return glf::checkError("End");
}

void sample::render()
{
	Renderer->bind(0, kueken::program::UNIFIED, Program);
	Renderer->bind(Assembler);
	Renderer->bind(Test);
	Renderer->bind(Blend);

	{
		Renderer->bind(RasterizerMultisample);
		Renderer->bind(RendertargetMultisample, kueken::rendertarget::EXEC);
		Renderer->exec(Clear);

		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		
		glm::mat4 ViewTranslateZ = glm::translate(glm::mat4(1.0f), 0.0f, 0.0f, -tranlationCurrent.y);
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslateZ, rotationCurrent.y,-1.f, 0.f, 0.f);
		glm::mat4 ViewRotateY = glm::rotate(ViewRotateX, rotationCurrent.x, 0.f, 1.f, 0.f);
		glm::mat4 View = ViewRotateY;

		glm::mat4 ModelA = glm::translate(0.0f, 0.0f, 0.1f);
		glm::mat4 ModelB = glm::translate(0.0f, 0.0f,-0.5f);

		glm::mat4 MVP[2];
		MVP[0] = Projection * View * ModelA;
		MVP[1] = Projection * View * ModelB;

		kueken::buffer::object* Uniform = Renderer->map(UniformBuffer);
		Uniform->set(0, sizeof(MVP), &MVP[0][0]);
		Renderer->unmap(UniformBuffer);
		VariableMVP.set(UniformBuffer);
		VariableDiffuse.set(0);

		Renderer->bind(0, kueken::sampler::SAMPLER, SamplerDiffuse);
		Renderer->bind(0, kueken::texture::IMAGE2D, TextureDiffuse);
		Renderer->exec(DrawMesh);

		Renderer->bind(RendertargetMultisample, kueken::rendertarget::READ);
		Renderer->bind(RendertargetResolver, kueken::rendertarget::DRAW);
		Renderer->exec(Blit);
	}

	{
		glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		kueken::buffer::object* Uniform = Renderer->map(UniformBuffer);
		Uniform->set(0, sizeof(glm::mat4), &MVP[0][0]);
		Renderer->unmap(UniformBuffer);
		VariableMVP.set(UniformBuffer);
		VariableDiffuse.set(0);

		Renderer->bind(RasterizerSplash);
		Renderer->bind(RendertargetFramebuffer, kueken::rendertarget::EXEC);
		Renderer->exec(Clear);

		Renderer->bind(0, kueken::sampler::SAMPLER, SamplerSplash);
		Renderer->bind(0, kueken::texture::IMAGE2D, TextureRendertarget);

		Renderer->exec(DrawSplash);
	}

	glf::checkError("Render");
}

bool sample::initBlend()
{
	{
		kueken::blend::creator Creator;
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

bool sample::initBlit()
{
	{
		kueken::blit::creator Creator;
		Creator.setRectSrc(glm::ivec4(0, 0, windowSize));
		Creator.setRectDst(glm::ivec4(0, 0, windowSize));
		Creator.setMask(true, false, false);
		Creator.setFilter(kueken::blit::NEAREST);
		Blit = Renderer->create(Creator);
	}

	return glf::checkError("initBlit");
}

bool sample::initClear()
{
	{
		kueken::clear::creator Creator;
		Creator.setColor(glm::vec4(0.0f));
		Creator.setDepth(1.0f);
		Clear = Renderer->create(Creator);
	}

	return glf::checkError("initClear");
}

bool sample::initDraw()
{
	{
		kueken::draw::creator Creator;
		Creator.setFirst(0);
		Creator.setCount(mesh.elementCount());
		Creator.setInstances(1);
		DrawSplash = Renderer->create(Creator);
	}

	{
		kueken::draw::creator Creator;
		Creator.setFirst(0);
		Creator.setCount(mesh.elementCount());
		Creator.setInstances(2);
		DrawMesh = Renderer->create(Creator);
	}

	return glf::checkError("initDraw");
}

bool sample::initTexture2D()
{
	{
		kueken::texture::creator Creator;
		Creator.setTarget(kueken::texture::IMAGE2D);
		Creator.setFormat(kueken::texture::RGBA8);
		Creator.setImage(0, glm::uvec3(windowSize, glm::uint(1)), 0);
		TextureRendertarget = Renderer->create(Creator);
	}

	{
		gli::image ImageFile = gli::import_as(TEXTURE_DIFFUSE);

		kueken::texture::creator Creator;
		Creator.setFormat(kueken::texture::RGB8);
		Creator.setTarget(kueken::texture::IMAGE2D);
		for(kueken::texture::level Level = 0; Level < ImageFile.levels(); ++Level)
		{
			Creator.setImage(
				Level, 
				ImageFile[Level].dimensions(),
				ImageFile[Level].data());
		}

		TextureDiffuse = Renderer->create(Creator);
		kueken::texture::object * TextureDiffuseObject = Renderer->map(TextureDiffuse);
		TextureDiffuseObject->generateMipmaps();
		Renderer->unmap(TextureDiffuse);
	}


	return glf::checkError("initTexture2D");
}

bool sample::initTest()
{
	{
		kueken::test::creator Creator;
		Creator.setDepthEnable(true);
		Creator.setDepthFunc(kueken::test::LEQUAL);
		Test = Renderer->create(Creator);
	}

	return glf::checkError("initTest");
}

bool sample::initRasterizer()
{
	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator;
		Creator.setViewport(glm::ivec4(0, 0, windowSize));
		Creator.setScissor(false, glm::ivec4(windowSize >> 2, windowSize >> 1));
		Creator.setMultisample(true);
		RasterizerMultisample = Renderer->create(Creator);
	}

	{
		kueken::rasterizer::creator<kueken::rasterizer::POLYGON> Creator;
		Creator.setViewport(glm::ivec4(0, 0, windowSize));
		Creator.setScissor(false, glm::ivec4(windowSize >> 2, windowSize >> 1));
		Creator.setMultisample(false);
		RasterizerSplash = Renderer->create(Creator);
	}

	return glf::checkError("initRasterizer");
}

bool sample::initSampler()
{
	{
		kueken::sampler::creator Creator;
		Creator.setFilter(kueken::sampler::TRILINEAR);
		Creator.setWrap(kueken::sampler::REPEAT, kueken::sampler::REPEAT, kueken::sampler::REPEAT);
		Creator.setAnisotropy(16.f);
		SamplerDiffuse = Renderer->create(Creator);
	}

	{
		kueken::sampler::creator Creator;
		Creator.setFilter(kueken::sampler::NEAREST);
		Creator.setWrap(kueken::sampler::REPEAT, kueken::sampler::REPEAT, kueken::sampler::REPEAT);
		Creator.setAnisotropy(1.f);
		SamplerSplash = Renderer->create(Creator);
	}

	return glf::checkError("initSampler");
}

bool sample::initProgram()
{
	kueken::program::creator Creator;
	Creator.addSource(
		kueken::program::VERTEX, 
		kueken::program::FILE,
		VERTEX_SHADER_SOURCE);
	Creator.addSource(
		kueken::program::FRAGMENT, 
		kueken::program::FILE,
		FRAGMENT_SHADER_SOURCE);
	Program = Renderer->create(Creator);

	kueken::program::object* Object = Renderer->map(Program);
	VariablePosition = Object->get("Position", kueken::program::ATTRIB);
	VariableTexcoord = Object->get("Texcoord", kueken::program::ATTRIB);
	VariableDiffuse = Object->get("Diffuse", kueken::program::SAMPLER);
	VariableMVP = Object->get("MVP", kueken::program::UNIFORM);
	Renderer->unmap(Program);

	VariableMVP.set(UniformBuffer);

	return glf::checkError("initProgram");
}

bool sample::initAssembler()
{
	{
		kueken::assembler::creator Creator;

		Creator.setPrimitive(
			kueken::assembler::TRIANGLES);

		Creator.addAttribute(
			ArrayBuffer, 
			VariablePosition, 
			kueken::assembler::F32VEC2,
			mesh.stride(glv::SLOT0),
			mesh.offset(glv::POSITION2));

		Creator.addAttribute(
			ArrayBuffer, 
			VariableTexcoord, 
			kueken::assembler::F32VEC2,
			mesh.stride(glv::SLOT0),
			mesh.offset(glv::TEXCOORD));

		Creator.setElements(
			kueken::assembler::U16, 
			ElementBuffer);

		Assembler = Renderer->create(Creator);
	}

	return glf::checkError("initAssembler");
}

bool sample::initUniformBuffer()
{
	kueken::buffer::creator Creator;
	Creator.setSize(sizeof(glm::mat4) * 2);
	Creator.setData(0);
	Creator.setUsage(kueken::buffer::STREAM_READ);
	UniformBuffer = Renderer->create(Creator);

	return glf::checkError("initUniformBuffer");
}

bool sample::initArrayBuffer()
{
	mesh = glv::buildPlane(glv::ELEMENT_BIT | glv::POSITION2_BIT | glv::TEXCOORD_BIT, glm::vec2(1.0f));

	{
		kueken::buffer::creator Creator;
		Creator.setSize(mesh.vertexSize(glv::SLOT0));
		Creator.setData(mesh.vertexData(glv::SLOT0));
		Creator.setUsage(kueken::buffer::STATIC_DRAW);
		ArrayBuffer = Renderer->create(Creator);
	}

	{
		kueken::buffer::creator Creator;
		Creator.setSize(mesh.elementSize());
		Creator.setData(mesh.elementData());
		Creator.setUsage(kueken::buffer::STATIC_DRAW);
		ElementBuffer = Renderer->create(Creator);
	}

	return glf::checkError("initArrayBuffer");
}

bool sample::initRendertarget()
{
	{
		kueken::rendertarget::creator<kueken::rendertarget::CUSTOM> Creator;
		Creator.setTexture(kueken::rendertarget::COLOR0, TextureRendertarget, 0);
		RendertargetResolver = Renderer->create(Creator);
	}

	{
		kueken::renderbuffer::creator RenderbufferDepthCreator;
		RenderbufferDepthCreator.setFormat(kueken::renderbuffer::DEPTH24);
		RenderbufferDepthCreator.setSize(glm::uvec2(windowSize));
		RenderbufferDepthCreator.setSamples(4);
		kueken::renderbuffer::name RenderbufferDepth = Renderer->create(RenderbufferDepthCreator);

		kueken::renderbuffer::creator RenderbufferColor0Creator;
		RenderbufferColor0Creator.setFormat(kueken::renderbuffer::RGBA);
		RenderbufferColor0Creator.setSize(glm::uvec2(windowSize));
		RenderbufferColor0Creator.setSamples(4);
		kueken::renderbuffer::name RenderbufferColor0 = Renderer->create(RenderbufferColor0Creator);

		kueken::rendertarget::creator<kueken::rendertarget::CUSTOM> RendertargetCreator;
		RendertargetCreator.setRenderbuffer(kueken::rendertarget::DEPTH, RenderbufferDepth);
		RendertargetCreator.setRenderbuffer(kueken::rendertarget::COLOR0, RenderbufferColor0);
		RendertargetMultisample = Renderer->create(RendertargetCreator);
	}

	{
		kueken::rendertarget::creator<kueken::rendertarget::FRAMEBUFFER> Creator;
		RendertargetFramebuffer = Renderer->create(Creator);
	}

	return glf::checkError("initRendertarget");
}

int main(int argc, char* argv[])
{
	glm::ivec2 ScreenSize = glm::ivec2(640, 480);

	sample * Sample = new sample(SAMPLE_NAME, ScreenSize);
	if(Sample->check())
	{
		Sample->begin(ScreenSize);
		do
		{
			Sample->render();
		}
		while(Sample->event());
		Sample->end();

		delete Sample;
		return 0;
	}

	delete Sample;
	return 1;
}
