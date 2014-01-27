#include "GPUFence.h"

namespace oplo
{

	GPUFence::GPUFence() : m_fence(0)
	{}

	void GPUFence::createFence()
	{
#pragma message("TODO: log error")

		m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	void GPUFence::destroy()
	{
		glDeleteSync(m_fence);
		m_fence = 0;
	}

	void GPUFence::reset()
	{
		destroy();
		createFence();
	}

	unsigned GPUFence::wait(unsigned nano)
	{
		return glClientWaitSync(m_fence, 0, nano);
	}

}
