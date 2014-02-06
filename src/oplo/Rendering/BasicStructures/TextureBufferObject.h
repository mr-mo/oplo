#ifndef TEXTURE_BUFFER_OBJECT_H
#define TEXTURE_BUFFER_OBJECT_H

#include "BufferObject.h"

namespace oplo
{

	class TextureBufferObject
	{
	public:

		TextureBufferObject();

		void deallocate();

		void create(const char* label = 0);

		void bindBufferToTexture(unsigned fmt);

		BufferObject& getUnderlyingBuffer();

		unsigned getTextureId() const;

	private:

		unsigned m_id;

		BufferObject m_buffer;
	};

}

#endif
