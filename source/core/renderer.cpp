#include <core/renderer.hpp>

namespace kueken
{
	GLenum buffer_target_cast(kueken::buffer::target Target)
	{
		static GLenum const CastBufferTarget[kueken::buffer::TARGET_MAX] =
		{
			GL_ARRAY_BUFFER,			// ARRAY
			GL_ELEMENT_ARRAY_BUFFER,	// ELEMENT
			GL_UNIFORM_BUFFER_EXT,		// UNIFORM
			GL_TEXTURE_BUFFER_EXT,		// TEXTURE
			GL_PIXEL_PACK_BUFFER,		// PIXEL_PACK 
			GL_PIXEL_UNPACK_BUFFER,		// PIXEL_UNPACK
			GL_TRANSFORM_FEEDBACK_BUFFER_EXT// FEEDBACK
		};

		return CastBufferTarget[Target];
	}

	renderer::renderer()
	{}

	renderer::~renderer()
	{
		manager::destroy();
	}

	///////////////////////////
	// assembler

	assembler::name renderer::create(assembler::creator const & Creator)
	{
		assembler::creator Copy = Creator;
		Copy.setRenderer(this);

		assembler::object * Object = new assembler::object(Copy);
		return manager::instance().Assembler.reserve(Object);
	}

	void renderer::free(assembler::name& Name)
	{
		manager::instance().Assembler.release(Name);
	}

	void renderer::bind
	(
		assembler::name const & Name
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Assembler.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		kueken::assembler::object & Object = Manager.Assembler.setCurrentObject(0, Name);
		Object.bind();
		Primitive = Object.getPrimitive();
		ElementFormat = Object.getElementFormat();
	}

	///////////////////////////
	// blend

	blend::name renderer::create(blend::creator const & Creator)
	{
		blend::object * Object = new blend::object(Creator);
		return manager::instance().Blend.reserve(Object);
	}

	void renderer::free(blend::name& Name)
	{
		manager::instance().Blend.release(Name);
	}

	void renderer::bind
	(
		blend::name const & Name
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Blend.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		Manager.Blend.setCurrentObject(0, Name).bind();
	}

	///////////////////////////
	// blit

	blit::name renderer::create(blit::creator const & Creator)
	{
		blit::object * Object = new blit::object(Creator);
		return manager::instance().Blit.reserve(Object);
	}

	void renderer::free(blit::name& Name)
	{
		manager::instance().Blit.release(Name);
	}

	void renderer::exec
	(
		blit::name const & Name
	)
	{
		manager::instance().Blit.getObject(Name).exec();
	}
	///////////////////////////
	// buffer

	buffer::name renderer::create(buffer::creator const & Creator)
	{
		buffer::object * Object = new buffer::object(Creator);
		return manager::instance().Buffer.reserve(Object);
	}

	void renderer::free(buffer::name& Name)
	{
		manager::instance().Buffer.release(Name);
	}

	void renderer::bind
	(
		buffer::name const & Name,
		buffer::target const & Target
	)
	{
		manager& Manager = manager::instance();

		if(Name == buffer::name::Null())
		{
			glBindBuffer(buffer_target_cast(Target), 0);
			return;
		}
		
#if KUEKEN_REDUCE_CHANGES
		if(Manager.Buffer.isCurrent(Target, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		Manager.Buffer.setCurrentObject(Target, Name).bind(Target);
	}

	buffer::object* renderer::map(buffer::name& Name)
	{
		return manager::instance().Buffer.mapObject(Name);
	}

	void renderer::unmap(buffer::name& Name)
	{
		manager::instance().Buffer.unmapObject(Name);
	}

	///////////////////////////
	// clear

	clear::name renderer::create
	(	
		clear::creator const & Creator
	)
	{
		//return manager::instance().Clear.create(Creator);
		clear::object * Object = new clear::object(Creator);
		return manager::instance().Clear.reserve(Object);
	}

	void renderer::free
	(
		clear::name& Name
	)
	{
		manager::instance().Clear.release(Name);
	}

	void renderer::exec
	(
		clear::name const & Name
	)
	{
		manager::instance().Clear.getObject(Name).exec();
	}

	///////////////////////////
	// draw

	draw::name renderer::create
	(	
		draw::creator const & Creator
	)
	{
		//return manager::instance().Draw.create(Creator);
		draw::object * Object = new draw::object(Creator);
		return manager::instance().Draw.reserve(Object);
	}

	void renderer::free
	(
		draw::name& Name
	)
	{
		manager::instance().Draw.release(Name);
	}

	void renderer::exec
	(
		draw::name const & Name
	)
	{
		manager::instance().Draw.getObject(Name).exec(Primitive, ElementFormat);
	}

	draw::object* renderer::map(draw::name& Name)
	{
		return manager::instance().Draw.mapObject(Name);
	}

	void renderer::unmap(draw::name& Name)
	{
		manager::instance().Draw.unmapObject(Name);
	}

	///////////////////////////
	// image

	image::name renderer::create
	(
		image::creator const & Creator
	)
	{
		//return manager::instance().Image.create(Creator);
		image::object * Object = new image::object(Creator);
		return manager::instance().Image.reserve(Object);
	}

	void renderer::free
	(
		image::name& Name
	)
	{
		manager::instance().Image.release(Name);
	}

	image::object* renderer::map(image::name& Name)
	{
		return manager::instance().Image.mapObject(Name);
	}

	void renderer::unmap(image::name& Name)
	{
		manager::instance().Image.unmapObject(Name);
	}

	///////////////////////////
	// program

	program::name renderer::create(program::creator const & Creator)
	{
		program::object * Object = new program::object(Creator);
		return manager::instance().Program.reserve(Object);
		//return manager::instance().Program.create(Creator);
	}

	void renderer::free(program::name& Name)
	{
		manager::instance().Program.release(Name);
	}

	void renderer::bind(program::name const & Name)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Program.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		Manager.Program.setCurrentObject(0, Name).bind();
	}

	program::object* renderer::map(program::name& Name)
	{
		return manager::instance().Program.mapObject(Name);
	}

	void renderer::unmap(program::name& Name)
	{
		manager::instance().Program.unmapObject(Name);
	}

	///////////////////////////
	// query

	query::name renderer::create(query::creator const & Creator)
	{
		query::object * Object = new query::object(Creator);
		return manager::instance().Query.reserve(Object);
	}

	void renderer::free(query::name& Name)
	{
		manager::instance().Query.release(Name);
	}

	void renderer::begin(query::name const & Name, query::use const & Use)
	{
		manager& Manager = manager::instance();
		Manager.Query.getObject(Name).begin(Use);
	}

	void renderer::end(query::name const & Name, query::use const & Use)
	{
		manager& Manager = manager::instance();
		Manager.Query.getObject(Name).end(Use);
	}

	glm::uint64 renderer::get(query::name const & Name)
	{
		manager& Manager = manager::instance();
		return Manager.Query.getObject(Name).get();
	}

	///////////////////////////
	// rasterizer

	template <>
	rasterizer::name renderer::create
	(	
		rasterizer::creator<rasterizer::POLYGON> const & Creator
	)
	{
		rasterizer::object * Object = new rasterizer::object(Creator);
		return manager::instance().Rasterizer.reserve(Object);
		//return manager::instance().Rasterizer.create(Creator);
	}

	template <>
	rasterizer::name renderer::create
	(	
		rasterizer::creator<rasterizer::LINE> const & Creator
	)
	{
		rasterizer::object * Object = new rasterizer::object(Creator);
		return manager::instance().Rasterizer.reserve(Object);
		//return manager::instance().Rasterizer.create(Creator);
	}

	template <>
	rasterizer::name renderer::create
	(	
		rasterizer::creator<rasterizer::POINT> const & Creator
	)
	{
		rasterizer::object * Object = new rasterizer::object(Creator);
		return manager::instance().Rasterizer.reserve(Object);
		//return manager::instance().Rasterizer.create(Creator);
	}

	void renderer::free
	(
		rasterizer::name& Name
	)
	{
		manager::instance().Rasterizer.release(Name);
	}

	void renderer::bind
	(
		rasterizer::name const & Name
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Rasterizer.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		Manager.Rasterizer.setCurrentObject(0, Name).bind();
	}

	///////////////////////////
	// readpixels

	readpixels::name renderer::create(readpixels::creator const & Creator)
	{
		readpixels::object * Object = new readpixels::object(Creator);
		return manager::instance().ReadPixels.reserve(Object);
	}

	void renderer::free(readpixels::name& Name)
	{
		manager::instance().ReadPixels.release(Name);
	}

	void renderer::exec(readpixels::name const & Name)
	{
		manager::instance().ReadPixels.getObject(Name).exec();
	}

	///////////////////////////
	// renderbuffer

	renderbuffer::name renderer::create(renderbuffer::creator const & Creator)
	{
		renderbuffer::object * Object = new renderbuffer::object(Creator);
		return manager::instance().Renderbuffer.reserve(Object);
		//return manager::instance().Renderbuffer.create(Creator);
	}

	void renderer::free(renderbuffer::name& Name)
	{
		manager::instance().Renderbuffer.release(Name);
	}

	///////////////////////////
	// rendertarget

	template <>
	rendertarget::name renderer::create
	(
		rendertarget::creator<rendertarget::FRAMEBUFFER> const & Creator
	)
	{
		rendertarget::object * Object = new rendertarget::object(Creator);
		return manager::instance().Rendertarget.reserve(Object);
	}

	template <>
	rendertarget::name renderer::create
	(
		rendertarget::creator<rendertarget::CUSTOM> const & Creator
	)
	{
		rendertarget::object * Object = new rendertarget::object(Creator);
		return manager::instance().Rendertarget.reserve(Object);
		//return manager::instance().Rendertarget.create(Creator);
	}

	void renderer::free(rendertarget::name& Name)
	{
		manager::instance().Rendertarget.release(Name);
	}

	void renderer::bind
	(
		rendertarget::name const & Name,
		rendertarget::target const & Target
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Rendertarget.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		Manager.Rendertarget.setCurrentObject(0, Name).bind(Target);
	}

	///////////////////////////
	// sampler

	sampler::name renderer::create
	(
		sampler::creator const & Creator
	)
	{
		sampler::object * Object = new sampler::object(Creator);
		return manager::instance().Sampler.reserve(Object);
		//return manager::instance().Sampler.create(Creator);
	}

	void renderer::free
	(
		sampler::name& Name
	)
	{
		manager::instance().Sampler.release(Name);
	}

	///////////////////////////
	// test

	test::name renderer::create
	(
		test::creator const & Creator
	)
	{
		test::object * Object = new test::object(Creator);
		return manager::instance().Test.reserve(Object);
		//return manager::instance().Test.create(Creator);
	}

	void renderer::free
	(
		test::name& Name
	)
	{
		manager::instance().Test.release(Name);
	}

	void renderer::bind
	(
		test::name const & Name
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Test.isCurrent(0, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES
			
		Manager.Test.setCurrentObject(0, Name).bind();
	}

	///////////////////////////
	// texture

	template <>
	texture::name renderer::create
	(
		texture::creator<texture::buffer> const & Creator
	)
	{
		texture::object * Object = new texture::detail::objectBuffer(Creator);
		return manager::instance().Texture.reserve(Object);
		//return manager::instance().Texture.create(Creator);
	}

	template <>
	texture::name renderer::create
	(
		texture::creator<texture::image> const & Creator
	)
	{
		texture::object * Object = new texture::detail::objectImage(Creator);
		return manager::instance().Texture.reserve(Object);
		//return manager::instance().Texture.create(Creator);
	}

	void renderer::free
	(
		texture::name& Name
	)
	{
		manager::instance().Texture.release(Name);
	}

	void renderer::bind
	(
		texture::name const & Name,
		texture::slot Slot
	)
	{
		manager& Manager = manager::instance();

#if KUEKEN_REDUCE_CHANGES
		if(Manager.Texture.isCurrent(Slot, Name))
			return;
#endif//KUEKEN_REDUCE_CHANGES

		assert(glGetError() == GL_NO_ERROR);
		texture::object& Object = Manager.Texture.setCurrentObject(Slot, Name);
		assert(glGetError() == GL_NO_ERROR);
		Object.bind(Slot);
		assert(glGetError() == GL_NO_ERROR);
	}

}//namespace kueken