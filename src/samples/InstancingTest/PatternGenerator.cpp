#include "PatternGenerator.h"
#include <random>

int PatternGenerator::getTotalAllocations(int pattern, int baseScale /*= 750*/)
{
	switch (pattern)
	{
	case CUBE_JUNGLE:
		return baseScale;
	case CUBE_SPIRAL:
		return baseScale * 6;
	}
}

void PatternGenerator::fillBuffer(CubeData* buffer, int pattern, int baseScale)
{
	switch (pattern)
	{
	case CUBE_JUNGLE:
		makeCubes(baseScale, buffer);
		break;
	case CUBE_SPIRAL:
		makeSpirals(baseScale, buffer);
		break;
	}
}


void PatternGenerator::makeCubes(int numCubes, PatternGenerator::CubeData* cubes)
{
	std::random_device rd;
	std::mt19937 twister(rd());
	std::uniform_real_distribution<float> translation(-200, 200);
	std::uniform_real_distribution<float> rotation(0, 360);
	std::uniform_real_distribution<float> axisRange(-1, 1);
	std::uniform_real_distribution<float> color(0, 1);
	std::uniform_real_distribution<float> scale(0.1f, 5);

	for (int i = 0; i < numCubes; ++i)
	{
		oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));
		oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));

		cubes[i].m_translation = oplo::Vec3F(translation(twister), translation(twister), translation(twister));
		cubes[i].m_scale = scale(twister);
		cubes[i].m_color = oplo::Vec4F(color(twister), color(twister), color(twister), color(twister));
		cubes[i].m_rotation.fromAxisAngle(angle, axis);
	}
}

void PatternGenerator::makeSpirals(int numCubes, PatternGenerator::CubeData* cubes)
{
	std::random_device rd;
	std::mt19937 twister(rd());
	std::uniform_real_distribution<float> translation(-200, 200);
	std::uniform_real_distribution<float> rotation(0, 360);
	std::uniform_real_distribution<float> axisRange(-1, 1);
	std::uniform_real_distribution<float> color(0, 1);
	std::uniform_real_distribution<float> scale(0.5, 5);

	oplo::Vec4F colorV[3] =
	{
		oplo::Vec4F(255 / 255., 243 / 255., 0 / 255., 1),
		oplo::Vec4F(1, 0.7, 0, 1),
		oplo::Vec4F(1, 76 / 255., 0, 1)
	};

	float translationScalars[] = { 200, 125, 50 };

	for (int i = 0; i < numCubes; ++i)
	{
		double freqZ = 100;
		double angleR = (i / double(numCubes - 1)) * (3.1415926 * 2);

		for (int j = 0; j < 3; ++j)
		{
			oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

			oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
			oplo::Vec3F l0(sin(angleR) * translationScalars[j], cos(angleR) * translationScalars[j], 0);

			float ct = 0.1f * axisRange(twister);

			cubes[i * 3 + j].m_translation = l0;
			cubes[i * 3 + j].m_scale = scale(twister);
			cubes[i * 3 + j].m_color = colorV[j] + ct;
			cubes[i * 3 + j].m_rotation.fromAxisAngle(angle, axis);
		}
	}

	colorV[0] = oplo::Vec4F(124 / 255., 93 / 255., 178 / 255., 1);
	colorV[1] = oplo::Vec4F(25 / 255., 109 / 255., 255 / 255., 1);
	colorV[2] = oplo::Vec4F(93 / 255., 0 / 255., 255 / 255., 1);

	translationScalars[0] = 50;
	translationScalars[1] = 125;
	translationScalars[2] = 50;

	float sts[] = { 200, 30, 20 };

	for (int i = 0; i < numCubes; ++i)
	{
		double phaseOffset = 0; // (3.1415926 * 2) / 3;
		double angleR = (i / double(numCubes - 1)) * (3.1415926 * 2);
		double angleN = ((i + 1) / double(numCubes - 1)) * (3.1415926 * 2);

		for (int j = 0; j < 3; ++j)
		{
			oplo::Vec3F axis(axisRange(twister), axisRange(twister), axisRange(twister));
			oplo::Anglef angle = oplo::Anglef::fromDeg(rotation(twister));

			oplo::Vec3F tmp0(sin(angleR), cos(angleR), 0);
			tmp0.normalize();

			oplo::Vec3F tmp1(sin(angleN), cos(angleN), 0);
			tmp1.normalize();

			oplo::Vec3F l0 = tmp0 * 50.f;
			oplo::Vec3F l1 = tmp1 * 50.f;

			oplo::Vec3F l = l1 - l0;
			l.normalize();

			oplo::Quat<float> q;
			q.fromAxisAngle(oplo::Angle<float>((angleR)* 17), l);
			q.normalize();

			oplo::Vec3F d2(0, 0, 1);
			d2 = oplo::TransformVector(q, d2);

			float ct = 0.1f * axisRange(twister);

			cubes[numCubes * 3 + i * 3 + j].m_translation = d2 * sts[j] + l0;
			cubes[numCubes * 3 + i * 3 + j].m_scale = d2[2] < 0 ? 1.f : 0.f;
			cubes[numCubes * 3 + i * 3 + j].m_color = colorV[j] + ct;
			cubes[numCubes * 3 + i * 3 + j].m_rotation.fromAxisAngle(angle, axis);
		}
	}
}
