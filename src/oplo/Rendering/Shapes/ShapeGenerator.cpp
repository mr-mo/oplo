#include "ShapeGenerator.h"
#include "Math\Vector.h"

namespace oplo
{
	void MakeCube(Vec3F const& min, Vec3F const& max, float* buffer, int interleave)
	{
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;

		//bottom face
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = min[2];
		buffer += interleave;

		//back face
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;

		//front face
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;

		//left
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = min[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;

		//right
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = max[2];
		buffer += interleave;
		buffer[0] = max[0]; buffer[1] = min[1]; buffer[2] = min[2];
	}

	//-1 -> 1
	const float* MakeScreenQuad()
	{
		static float q[] =
		{
			-1, -1,
			1, -1,
			1, 1,
			-1, 1
		};

		return q;
	}

	//0 -> 1
	const float* MakeScreenQuad01()
	{
		static float q[] =
		{
			0, 0,
			1, 0,
			1, 1,
			0, 1
		};

		return q;
	}
}