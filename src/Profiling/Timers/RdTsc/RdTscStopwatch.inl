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

#ifdef OPLO_X64
#include <intrin.h>
#endif

void RdTscStopwatch::start()
{
	m_start.m_result = poll();
}

void RdTscStopwatch::stop()
{
	m_stop.m_result = poll();
}

int64_t RdTscStopwatch::runningDiff() const
{
	return poll() - m_start.m_result;
}

int64_t RdTscStopwatch::diff() const
{
	return m_stop.m_result - m_start.m_result;
}


inline void RdTscStopwatch::serialize()
{
	static int tmp[4];
	__cpuid(tmp, 0);
}

int64_t RdTscStopwatch::poll()
{
#ifdef OPLO_X64
	serialize();
	return __rdtsc();
#else
	TicksUnion value;

	__asm
	{
		pushad
		cpuid
		rdtsc
		mov value.m_lo, eax
		mov value.m_hi, edx
		popad
	}

	return value.m_result;
#endif
}