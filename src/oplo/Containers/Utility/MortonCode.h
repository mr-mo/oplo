#ifndef MORTON_CODE_H
#define MORTON_CODE_H

#include <cstdint>
#include "Math/Vector.h"

//3 dimensional morton code
namespace oplo
{

	// NN is unused
	// CCC morton triplet
	// DDD depth code

	//CCC CCC CCC CCC CCC CCC CCC CCC CCC 0 DDDD
	struct MortonCode
	{
	public:

		enum
		{
			MAX_LEVELS = 9
		};

		MortonCode();

		MortonCode(uint32_t c);

		inline bool operator==(const MortonCode& rhs) const
		{
			return m_code == rhs.m_code;
		}

		inline bool operator<(const MortonCode& rhs) const
		{
			const int d0 = getDepth();
			const int d1 = rhs.getDepth();
			return d0 != d1 ? d0 < d1 : m_code < rhs.m_code;
		}

		inline void clearCode()
		{
			m_code = 0;
		}

		inline unsigned getDepth() const
		{
			return m_code & 0x0000000F; // ^ 0xe0000000; 
		}

		inline void setDepth(unsigned d)
		{
			m_code &= ~0x0000000F;
			m_code |= d & 0x0000000F; // ^ 0xe0000000); 
		}

		inline uint32_t getCode() const
		{
			return m_code;
		}

		inline unsigned getCode(unsigned d) const
		{
			return (m_code & mortonHexLut[d]) >> (5 + (MAX_LEVELS - d - 1) * 3);
		}

		inline void setCode(unsigned d, unsigned c)
		{
			m_code &= ~mortonHexLut[d];
			m_code |= (c << (5 + (MAX_LEVELS - d - 1) * 3));
		}

		template<int INDEX> unsigned  getCode() const
		{
			return (m_code & mortonHexLut[INDEX]) >> (5 + (MAX_LEVELS - d - 1) * 3);
		}

		void print();

		static const unsigned getLevelMask(unsigned i)
		{
			return mortonHexLut[i];
		}

		static const Vector<double, 3>& getScalar(unsigned i)
		{
			return m_scalars[i];
		}

	private:

		uint32_t m_code;

		//000
		//001
		//010
		//011
		//100
		//101
		//110
		//111

		static const Vector<double, 3> m_scalars[8];

		static const unsigned mortonHexLut[MAX_LEVELS];
	};

}

#endif
