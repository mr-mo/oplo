#ifndef GPU_FENCE_H
#define GPU_FENCE_H

#include "Engine/ExitCodes.h"
#include "Rendering/OpenGL.h"

class GPUFence
{
public:

	GPUFence();

	ExitCode createFence();

	void reset();

	void destroy();

	GLenum wait(unsigned nano);

private:

	GLsync m_fence;
};


#endif
