#include "MortonCode.h"
#include <iostream>

namespace oplo
{

	const unsigned MortonCode::mortonHexLut[MAX_LEVELS] =
	{
		0xE0000000, //0
		0x1C000000, //1
		0x03800000, //2
		0x00700000, //3
		0x000E0000, //4
		0x0001C000, //5
		0x00003800, //6
		0x00000700, //7
		0x000000E0  //8
	};

	const Vector<double, 3> MortonCode::m_scalars[8] =
	{
		Vector<double, 3>(-1, -1, -1),
		Vector<double, 3>(-1, -1, 1),
		Vector<double, 3>(-1, 1, -1),
		Vector<double, 3>(-1, 1, 1),
		Vector<double, 3>(1, -1, -1),
		Vector<double, 3>(1, -1, 1),
		Vector<double, 3>(1, 1, -1),
		Vector<double, 3>(1, 1, 1)
	};

	//__m128 mortonCode::m_scalarsSIMD[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };


	MortonCode::MortonCode() :
		m_code(0)
	{}

	MortonCode::MortonCode(uint32_t c) :
		m_code(c)
	{}

	void MortonCode::print() 
	{

		unsigned numIters = 32;
		unsigned startNumber = 0x80000000;
		int c = 0;
		int bit = 0;

		for (; startNumber > 0; startNumber >>= 1, ++bit)
		{
			std::cout << (((m_code & startNumber) != 0 ? "1" : "0"));
			++c;

			if (c == 3 && bit < 27)
			{
				std::cout << " ";
				c = 0;
			}
		}

		std::cout << std::endl;
	}


	//void mortonCode::initializeScalars()
	//{
	//
	//	FORCE_ALIGN(16) float scalars[8][4] =
	//	{
	//		{ -1, -1, -1, 0 },
	//		{ -1, -1, 1, 0 },
	//		{ -1, 1, -1, 0 },
	//		{ -1, 1, 1, 0 },
	//		{ 1, -1, -1, 0 },
	//		{ 1, -1, 1, 0 },
	//		{ 1, 1, -1, 0 },
	//		{ 1, 1, 1, 0 }
	//	};
	//
	//	m_scalarsSIMD[0] = _mm_load_ps(scalars[0]);
	//	m_scalarsSIMD[1] = _mm_load_ps(scalars[1]);
	//	m_scalarsSIMD[2] = _mm_load_ps(scalars[2]);
	//	m_scalarsSIMD[3] = _mm_load_ps(scalars[3]);
	//
	//	m_scalarsSIMD[4] = _mm_load_ps(scalars[4]);
	//	m_scalarsSIMD[5] = _mm_load_ps(scalars[5]);
	//	m_scalarsSIMD[6] = _mm_load_ps(scalars[6]);
	//	m_scalarsSIMD[7] = _mm_load_ps(scalars[7]);
	//}

}
