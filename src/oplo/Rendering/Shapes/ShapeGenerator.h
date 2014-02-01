#ifndef OPLO_SHAPE_GENERATOR_H
#define OPLO_SHAPE_GENERATOR_H

class Vec3F;

namespace oplo
{
	void MakeCube(Vec3F const& min, Vec3F const& max, float* buffer, int interleave = 3);

	void CalculateNormalsForSurface(int surfaceType, float* buffer, int interleave = 3);

	//-1 -> 1
	const float* MakeScreenQuad();
	
	//0 -> 1
	const float* MakeScreenQuad01();
}

#endif
