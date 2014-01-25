#include "GPUFence.h"


GPUFence::GPUFence() : m_fence(0)
{}

ExitCode GPUFence::createFence()
{
	if (m_fence)
		return ExitCode::FAILURE;

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