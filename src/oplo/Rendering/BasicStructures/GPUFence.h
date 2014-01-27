#ifndef GPU_FENCE_H
#define GPU_FENCE_H

#include "Rendering/oploGL.h"

namespace oplo
{

	class GPUFence
	{
	public:

		GPUFence();

		void createFence();

		void reset();

		void destroy();

		GLenum wait(unsigned nano);

	private:

		GLsync m_fence;
	};

}

#endif
