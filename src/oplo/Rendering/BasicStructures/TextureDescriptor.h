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
			I16 = GL_SHORT,
			UI16 = GL_UNSIGNED_SHORT,
			I32 = GL_INT,
			UI32 = GL_UNSIGNED_INT,
			F32 = GL_FLOAT
		};

		TextureDescriptor();

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

		void createImmutable();

		void subloadMip(
			void* data,
			unsigned depth,
			unsigned x,
			unsigned y,
			unsigned z,
			unsigned width,
			unsigned height);

		void subload(
			void* data,
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
