#ifndef KUEKEN_RENDERER_INCLUDED
#define KUEKEN_RENDERER_INCLUDED

#include "manager.hpp"

namespace kueken
{
	class renderer
	{
	public:
		renderer();
		~renderer();

		program::version getVersion() const;

		blend::name create(blend::creator const & Creator);
		void free(blend::name & Name);
		void bind(blend::name const & Name);

		blit::name create(blit::creator const & Creator);
		void free(blit::name & Name);
		void exec(blit::name const & Name);

		buffer::name create(buffer::creator const & Creator);
		void free(buffer::name & Name);
		void bind(
			buffer::name const & Name,
			buffer::target const & Target);
		buffer::name getBinding(buffer::slot const & Slot) const;
		buffer::object & map(buffer::name const & Name);
		void unmap(buffer::name const & Name);

		clear::name create(clear::creator const & Creator);
		void free(clear::name & Name);
		void exec(clear::name const & Name);

		draw::name create(draw::creator<draw::ARRAY> const & Creator);
		draw::name create(draw::creator<draw::ELEMENT> const & Creator);
		void free(draw::name & Name);
		void exec(draw::name const & Name);
		draw::object & map(draw::name const & Name);
		void unmap(draw::name const & Name);

		template <framebuffer::type Type>
		framebuffer::name create(framebuffer::creator<Type> const & Creator);
		void free(framebuffer::name& Name);		
		void bind(
			framebuffer::name const & Name,
			framebuffer::target const & Target);

		layout::name create(layout::creator const & Creator);
		void free(layout::name & Name);
		void bind(
			layout::name const & Name);

		program::name create(program::creator const & Creator);
		void free(program::name& Name);
		void bind(
			program::name const & Name, 
			program::target const & Target);
		program::object & map(program::name const & Name);
		void unmap(program::name const & Name);

		query::name create(query::creator const & Creator);
		void free(query::name & Name);
		void begin(query::name const & Name, query::use const & Use);
		void end(query::name const & Name, query::use const & Use);
		glm::uint64 get(query::name const & Name);

		template <rasterizer::type Type>
		rasterizer::name create(rasterizer::creator<Type> const & Creator);
		void free(rasterizer::name & Name);
		void bind(
			rasterizer::name const & Name);

		sampler::name create(sampler::creator const & Creator);
		void free(sampler::name & Name);
		void bind(
			sampler::name const & Name,
			sampler::slot const & Slot);

		test::name create(test::creator const & Creator);
		void free(test::name & Name);
		void bind(
			test::name const & Name);

		//template <texture::type Type>
		//texture::name create(texture::creator<Type> const & Creator);
		//void free(texture::name& Name);		
		//void bind(texture::name const & Name, texture::slot Slot);

		readpixels::name create(readpixels::creator const & Creator);
		void free(readpixels::name & Name);
		void exec(readpixels::name const & Name);

		renderbuffer::name create(renderbuffer::creator const & Creator);
		void free(renderbuffer::name & Name);		

		texture::name create(texture::creator<texture::IMAGE> const & Creator);
		texture::name create(texture::creator<texture::BUFFER> const & Creator);
		void free(texture::name & Name);
		void bind(
			texture::name const & Name,
			texture::slot const & Slot);
		texture::object & map(texture::name const & Name);
		void unmap(texture::name const & Name);

		//template <typename OBJECT>
		//void info() const{assert(0);}

		//template <>
		//texture::info info<texture::object>() const
		//{
		//	return texture::info();
		//}

		//query::name create(query::creator const & Creator);
		//void free(query::name& Name);
		//void begin(query::name const & Name);
		//void end(query::name const & Name);
		//query::object* map(query::name& Name);
		//void unmap(query::name& Name);

		//feedback::name create(feedback::creator const & Creator);
		//void free(feedback::name& Name);
		//void bind(feedback::name const & Name);

		//sync::name create(sync::creator const & Creator);
		//void free(sync::name& Name);
		//void exec(sync::name const & Name);

		void reset();




		////! Draw calls
		//void Draw(glm::uint32 InstanceCount = 1);
		//void Clear(glm::uint32 Flags);
		//void Flush(); // Query and fence?
		//void Finish(); // Query and fence?
		////void Blit(); ? Framebuffer

		//// Error management
		//bool IsError();
		//std::string GetError() const;
		//bool Check(glm::uint32 Flags) const;

	private:
	};

}//namespace kueken

namespace core = kueken;

#include "renderer.inl"

#endif//KUEKEN_RENDERER_INCLUDED
