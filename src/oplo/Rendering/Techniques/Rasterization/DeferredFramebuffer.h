#ifndef DEFERRED_FRAMEBUFFER_H
#define DEFERRED_FRAMEBUFFER_H

#include "Rendering/BasicStructures/Framebuffer.h"

namespace oplo
{
	//Utility class
	class DeferredFramebuffer : public TextureFramebuffer<3>
	{
	public:

		DeferredFramebuffer();

		void enableAll();

		void resize(unsigned w, unsigned h);

		void setFormat(unsigned fmt, unsigned type);

		void initialize(int w, int h, int depthBufferBits);

	private:

		unsigned m_fmt;
		unsigned m_type;
	};

}

#endif
