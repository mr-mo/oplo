#include "Rendering/Techniques/Rasterization/DeferredFramebuffer.h"
#include "Rendering/BasicStructures/TextureDescriptor.h"

namespace oplo
{
	DeferredFramebuffer::DeferredFramebuffer() : TextureFramebuffer<3>("DeferredBuffer"), m_type(GL_FLOAT), m_fmt(GL_RGBA32F)
	{}

	void DeferredFramebuffer::enableAll()
	{
		GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		enableBuffers(3, bufs);
	}

	void DeferredFramebuffer::resize(unsigned w, unsigned h)
	{
		getDepthBuffer().resize(w, h);
		getAttachment(0).resize(w, h);
	}

	void DeferredFramebuffer::setFormat(unsigned fmt, unsigned type)
	{
		m_fmt = fmt;
		m_type = type;
	}

	void DeferredFramebuffer::initialize(int w, int h, int depthBufferBits)
	{
		create();
		std::shared_ptr<TextureDescriptor> desc = std::make_shared<TextureDescriptor>();
		desc->setup2d(GL_TEXTURE_2D, 0, depthBufferBits == 32 ? GL_DEPTH_COMPONENT32 : GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, desc->F32);
		desc->createObject();
		desc->createStorage(false);
		desc->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		desc->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		attach(GL_DEPTH_ATTACHMENT, 0, desc);

		desc = std::make_shared<TextureDescriptor>();
		desc->setup3d(GL_TEXTURE_2D_ARRAY, 0, m_fmt, w, h, 3, GL_RGBA, static_cast<TextureDescriptor::DataType>(m_type));
		desc->createObject();
		desc->createStorage(false);
		desc->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		desc->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		attach(GL_COLOR_ATTACHMENT0, 0, 0, desc);
		attach(GL_COLOR_ATTACHMENT1, 0, 1, desc);
		attach(GL_COLOR_ATTACHMENT2, 0, 2, desc);
	}

}