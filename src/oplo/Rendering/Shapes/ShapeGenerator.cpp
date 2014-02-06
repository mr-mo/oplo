#include "ShapeGenerator.h"
#include "Math\Vector.h"
#include "Rendering\oploGL.h"

#include <cassert>

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

	void InjectNormalsIntoPrimitive(int primitiveType, float* buffer, int vertexCount, int interleave )
	{
		Vec3F a, b, c;
		int verticesPerPrimitive = 0;

		switch (primitiveType)
		{
		case GL_TRIANGLES:
			verticesPerPrimitive = 3;
			break;
		case GL_QUADS:
			verticesPerPrimitive = 4;
			break;
		default:
			assert(0); //implement me
			break;
		}

		for (int i = 0; i < vertexCount * interleave; i += interleave * verticesPerPrimitive)
		{
			a.setXyz(buffer[i + 0], buffer[i + 1], buffer[i + 2]);
			b.setXyz(buffer[i + 0 + interleave], buffer[i + 1 + interleave], buffer[i + 2 + interleave]);
			c.setXyz(buffer[i + 0 + interleave * 2], buffer[i + 1 + interleave * 2], buffer[i + 2 + interleave * 2]);
			a -= c;
			b -= c;
			c = Cross(a, b);
			c.normalize();

			for (int j = 0; j < verticesPerPrimitive; ++j)
			{
				buffer[i + 3 + interleave * j + 0] = c[0];
				buffer[i + 3 + interleave * j + 1] = c[1];
				buffer[i + 3 + interleave * j + 2] = c[2];
			}
		}
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