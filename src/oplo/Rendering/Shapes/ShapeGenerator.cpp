#include "ShapeGenerator.h"
#include "Math\Vector.h"

namespace oplo
{
	void MakeCube(Vec3F const& min, Vec3F const& max, float* buffer)
	{
		buffer[0] = max[0]; buffer[1] = max[1]; buffer[2] = min[2];
		buffer[3] = min[0]; buffer[4] = max[1]; buffer[5] = min[2];
		buffer[6] = min[0]; buffer[7] = max[1]; buffer[8] = max[2];
		buffer[9] = max[0]; buffer[10] = max[1]; buffer[11] = max[2];

		//bottom face
		buffer[12] = max[0]; buffer[13] = min[1]; buffer[14] = max[2];
		buffer[15] = min[0]; buffer[16] = min[1]; buffer[17] = max[2];
		buffer[18] = min[0]; buffer[19] = min[1]; buffer[20] = min[2];
		buffer[21] = max[0]; buffer[22] = min[1]; buffer[23] = min[2];

		//back face
		buffer[24] = max[0]; buffer[25] = max[1]; buffer[26] = max[2];
		buffer[27] = min[0]; buffer[28] = max[1]; buffer[29] = max[2];
		buffer[30] = min[0]; buffer[31] = min[1]; buffer[32] = max[2];
		buffer[33] = max[0]; buffer[34] = min[1]; buffer[35] = max[2];

		//front face
		buffer[36] = max[0]; buffer[37] = min[1]; buffer[38] = min[2];
		buffer[39] = min[0]; buffer[40] = min[1]; buffer[41] = min[2];
		buffer[42] = min[0]; buffer[43] = max[1]; buffer[44] = min[2];
		buffer[45] = max[0]; buffer[46] = max[1]; buffer[47] = min[2];

		//left
		buffer[48] = min[0]; buffer[49] = max[1]; buffer[50] = max[2];
		buffer[51] = min[0]; buffer[52] = max[1]; buffer[53] = min[2];
		buffer[54] = min[0]; buffer[55] = min[1]; buffer[56] = min[2];
		buffer[57] = min[0]; buffer[58] = min[1]; buffer[59] = max[2];

		//right
		buffer[60] = max[0]; buffer[61] = max[1]; buffer[62] = min[2];
		buffer[63] = max[0]; buffer[64] = max[1]; buffer[65] = max[2];
		buffer[66] = max[0]; buffer[67] = min[1]; buffer[68] = max[2];
		buffer[69] = max[0]; buffer[70] = min[1]; buffer[71] = min[2];
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