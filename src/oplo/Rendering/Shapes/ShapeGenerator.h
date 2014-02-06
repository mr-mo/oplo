#ifndef OPLO_SHAPE_GENERATOR_H
#define OPLO_SHAPE_GENERATOR_H

#include "Math/Vector.h"

namespace oplo
{
	void MakeCube(Vec3F const& min, Vec3F const& max, float* buffer, int interleave = 3);

	//assumes buffer[0-3] = vertex data, buffer[4-6] is for normals
	void InjectNormalsIntoPrimitive(int primitiveType, float* buffer, int vertexCount, int interleave = 3);

	//-1 -> 1
	const float* MakeScreenQuad();
	
	//0 -> 1
	const float* MakeScreenQuad01();
}

#endif
