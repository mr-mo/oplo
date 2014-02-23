#include "TextureBufferObject.h"
#include "Rendering\Debug\RenderingDebugOutput.h"
#include <cstdio>

namespace oplo
{
	TextureBufferObject::TextureBufferObject() : m_id(0)
	{}

	void TextureBufferObject::deallocate()
	{
		m_buffer.deallocate();

		glDeleteTextures(1, &m_id);
	}

	void TextureBufferObject::create(const char* label /*= 0*/)
	{
		m_buffer.create(label);

		if (!m_id)
		{
			glGenTextures(1, &m_id);

			char buffer[128];

			if (label)
			{
				sprintf_s(buffer, "%s_bufTex", label);
			}
			else
			{
				sprintf_s(buffer, "Unknown_bufTex", label);
			}

			oplo::ObjectLabel(GL_BUFFER, m_id, -1, buffer);
		}
	}

	void TextureBufferObject::bindBufferToTexture(unsigned fmt)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, m_buffer.getId());
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		glBindTexture(GL_TEXTURE_BUFFER, m_id);
		glTexBuffer(GL_TEXTURE_BUFFER, fmt, m_buffer.getId());
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}

	BufferObject& TextureBufferObject::getUnderlyingBuffer()
	{
		return m_buffer;
	}

	unsigned TextureBufferObject::getTextureId() const
	{
		return m_id;
	}
}