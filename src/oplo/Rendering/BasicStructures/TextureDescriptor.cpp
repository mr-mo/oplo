#include "TextureDescriptor.h"
#include "BufferObject.h"

#include <cassert>

namespace oplo
{

	TextureDescriptor::TextureDescriptor() :
		m_id(0),
		m_target(0),
		m_width(0),
		m_height(0),
		m_mipmapDepth(0),
		m_zDepth(0),
		m_type(I8)
	{}

	TextureDescriptor::~TextureDescriptor()
	{
		destroyObject();
	}

	void TextureDescriptor::setup2d(
		unsigned target,
		unsigned mipmapDepth,
		unsigned internalFormat,
		unsigned width,
		unsigned height,
		unsigned format,
		DataType type)
	{
		m_width = width;
		m_height = height;
		m_zDepth = 1;
		m_mipmapDepth = mipmapDepth;
		m_type = type;
		m_target = target;
		m_internalFormat = internalFormat;
		m_format = format;
	}

	void TextureDescriptor::setup3d(
		unsigned target,
		unsigned mipmapDepth,
		unsigned internalFormat,
		unsigned width,
		unsigned height,
		unsigned depth,
		unsigned format,
		DataType type)
	{
		m_width = width;
		m_height = height;
		m_zDepth = depth;
		m_mipmapDepth = mipmapDepth;
		m_type = type;
		m_target = target;
		m_internalFormat = internalFormat;
		m_format = format;
	}

	void TextureDescriptor::createStorage(bool immutable)
	{
		if (immutable)
		{
			switch (m_target)
			{
			case GL_TEXTURE_2D:
				glTextureStorage2DEXT(m_id, m_target, m_mipmapDepth, m_internalFormat, m_width, m_height);
				break;
			case GL_TEXTURE_2D_ARRAY:
				glTextureStorage3DEXT(m_id, m_target, m_mipmapDepth, m_internalFormat, m_width, m_height, m_zDepth);
				break;
			}
		}
		else
		{
			switch (m_target)
			{
			case GL_TEXTURE_2D:
				glTextureImage2DEXT(m_id, m_target, m_mipmapDepth, m_internalFormat, m_width, m_height, 0, m_format, m_type, 0);
				break;
			case GL_TEXTURE_2D_ARRAY:
				glTextureImage3DEXT(m_id, m_target, m_mipmapDepth, m_internalFormat, m_width, m_height, m_zDepth, 0, m_format, m_type, 0);
				break;
			}
		}
	}

	void TextureDescriptor::resize(int w, int h)
	{
		m_width = w;
		m_height = h;

		//todo: error check against immutable
		createStorage(false);
	}

	void TextureDescriptor::load(const void* data)
	{
		subload(data, 0, 0, 0, m_width, m_height);
	}

	void TextureDescriptor::setParameter(unsigned texParameter, int value)
	{
		glTextureParameteriEXT(m_id, m_target, texParameter, value);
	}

	void TextureDescriptor::setParameter(unsigned texParameter, float value)
	{
		glTextureParameterfEXT(m_id, m_target, texParameter, value);
	}

	void TextureDescriptor::subloadMip(
		const void* data,
		unsigned depth,
		unsigned x,
		unsigned y,
		unsigned z,
		unsigned width,
		unsigned height)
	{
		switch (m_target)
		{
		case GL_TEXTURE_2D:
			glTextureSubImage2DEXT(m_id, m_target, depth, 0, 0, width, height, m_format, m_type, data);
			break;
		case GL_TEXTURE_2D_ARRAY:
			glTextureSubImage3DEXT(m_id, m_target, depth, x, y, z, m_width, m_height, 1, m_format, m_type, data);
			break;
			//case GL_TEXTURE_1D_ARRAY:
			//case GL_TEXTURE_3D:
			//case GL_TEXTURE_1D:
		default:
			assert(0); //implement me when needed
		}
	}


	void TextureDescriptor::subload(
		const void* data,
		unsigned x,
		unsigned y,
		unsigned z,
		unsigned width,
		unsigned height)
	{
		switch (m_target)
		{
		case GL_TEXTURE_2D:
			glTextureSubImage2DEXT(m_id, m_target, 0, x, y, width, height, m_format, m_type, data);
			break;
		case GL_TEXTURE_2D_ARRAY:
			glTextureSubImage3DEXT(m_id, m_target, 0, x, y, z, m_width, m_height, 1, m_format, m_type, data);
			break;
			//case GL_TEXTURE_1D_ARRAY:
			//case GL_TEXTURE_3D:
			//case GL_TEXTURE_1D:
		default:
			assert(0); //implement me when needed
		}
	}

	void TextureDescriptor::textureCopy(const TextureDescriptor& desc)
	{
		glCopyImageSubData(desc.getId(), desc.getTarget(), 0, 0, 0, 0, m_id, m_target, 0, 0, 0, 0, desc.getWidth(), desc.getHeight(), 0);
	}


	void TextureDescriptor::createObject()
	{
		glGenTextures(1, &m_id);
	}

	void TextureDescriptor::destroyObject()
	{
		glDeleteTextures(1, &m_id);
	}

	unsigned TextureDescriptor::getId() const
	{
		return m_id;
	}

	unsigned TextureDescriptor::getWidth() const
	{
		return m_width;
	}

	unsigned TextureDescriptor::getHeight() const
	{
		return m_height;
	}

	unsigned TextureDescriptor::getTarget() const
	{
		return m_target;
	}

}
