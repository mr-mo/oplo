/*The MIT License(MIT)

Copyright(c) 2014 Erik Scorelle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/


#ifndef OPLO_PROF_TIMER_H
#define OPLO_PROF_TIMER_H

#include <stdint.h>
#include <intrin.h>

class ProfStopwatch
{
public:

	typedef int64_t ReturnType;

	ProfStopwatch()
	{
		m_start.m_result = 0;
		m_stop.m_result = 0;
	}

	inline void start()
	{
		m_start.m_result = pollS();
	}

	inline void stop()
	{
		m_stop.m_result = pollE();
	}

	inline int64_t runningDiff() const
	{
		return pollE() - m_start.m_result;
	}

	inline int64_t diff() const
	{
		return m_stop.m_result - m_start.m_result;
	}

	inline static int64_t poll()
	{
		static unsigned int aux;
		return __rdtscp(&aux);
	}

	inline static int64_t pollS()
	{
		static int in[4];
		static int x;
		__cpuid(in, x);
		return __rdtsc();
	}

	inline static int64_t pollE()
	{
		int64_t o = poll();

		static int in[4];
		static int x;
		__cpuid(in, x);

		return o;
	}

private:

	union TicksUnion
	{
		struct
		{
			int32_t m_lo, m_hi;
		};

		int64_t m_result;
	};

	TicksUnion m_start;
	TicksUnion m_stop;
};

#endif
