#ifndef TEXTURE_DESCRIPTOR_H
#define TEXTURE_DESCRIPTOR_H

#include "Rendering/oploGL.h"

namespace oplo
{

	class BufferObject;
	class TextureDescriptor
	{
	public:

		enum DataType
		{
			I8 = GL_BYTE,
			UI8 = GL_UNSIGNED_BYTE,
			F16 = GL_HALF_FLOAT,
			I16 = GL_SHORT,
			UI16 = GL_UNSIGNED_SHORT,
			I32 = GL_INT,
			UI32 = GL_UNSIGNED_INT,
			F32 = GL_FLOAT
		};

		TextureDescriptor();

		~TextureDescriptor();

		void textureCopy(const TextureDescriptor& desc);

		void setup2d(
			unsigned target,
			unsigned mipMapdepth,
			unsigned internalFormat,
			unsigned width,
			unsigned height,
			unsigned format,
			DataType type);

		void setup3d(
			unsigned target,
			unsigned mipMapdepth,
			unsigned internalFormat,
			unsigned width,
			unsigned height,
			unsigned depth,
			unsigned format,
			DataType type);

		void resize(int w, int h);

		void createStorage(bool immutable);

		void load(const void* data);

		void subloadMip(
			const void* data,
			unsigned depth,
			unsigned x,
			unsigned y,
			unsigned z,
			unsigned width,
			unsigned height);

		void subload(
			const void* data,
			unsigned x,
			unsigned y,
			unsigned z,
			unsigned width,
			unsigned height);

		void setParameter(unsigned texParameter, int value);

		void setParameter(unsigned texParameter, float value);

		void createObject();

		void destroyObject();

		unsigned getId() const;

		unsigned getWidth() const;

		unsigned getHeight() const;

		unsigned getTarget() const;

		template<typename T>
		void fetchTexture(T* ptr, int mip)
		{
			glGetTextureImageEXT(m_id, m_target, mip, m_format, m_type, ptr);
		}


	private:

		unsigned m_id;

		unsigned m_target;

		unsigned
			m_width,
			m_height,
			m_zDepth,
			m_mipmapDepth;

		unsigned m_internalFormat;

		unsigned m_format;

		DataType m_type;
	};

}

#endif
