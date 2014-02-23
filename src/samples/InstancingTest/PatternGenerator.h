#ifndef OPLO_INSTANCING_PATTERN_GENERATOR_H
#define OPLO_INSTANCING_PATTERN_GENERATOR_H

#include "Math\Quat.h"
#include "Math\Vector.h"

class PatternGenerator
{
public:

	enum
	{
		CUBE_JUNGLE = 0,
		CUBE_SPIRAL
	};

	struct CubeData
	{
		float m_scale;
		oplo::Vec3F m_translation;
		oplo::Quat<float> m_rotation;
		oplo::Vec4F m_color;
	};

	static_assert(sizeof(CubeData) == (sizeof(float)* 12), "Must have proper alignment");

	int getTotalAllocations(int pattern, int baseScale = 750);

	void fillBuffer(CubeData* buffer, int pattern, int baseScale);

private:

	void makeCubes(int baseScale, CubeData* cubes);

	void makeSpirals(int numCubes, CubeData* cubes);

};

#endif
