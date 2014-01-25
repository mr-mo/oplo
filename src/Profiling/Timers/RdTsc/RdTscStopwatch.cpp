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

#include <Profiling/Timers/RdTsc/RdTscStopwatch.h>

#ifdef OPLO_X64
#include <intrin.h>
#endif

RdTscStopwatch::RdTscStopwatch()
{
	m_start.m_result = 0;
	m_stop.m_result = 0;
}

int64_t RdTscStopwatch::calculateSingleTimeOverhead()
{
	TicksUnion value;
#ifdef OPLO_X64
	__int64 x0 = poll();
	__int64 x1 = poll();
	return x1 - x0;
#else
	__asm
	{
		pushad
		cpuid
		rdtsc
		mov value.m_lo, eax
		mov value.m_hi, edx
		popad

		pushad
		cpuid
		rdtsc
		sub eax, value.m_lo
		mov value.m_lo, eax
		popad
	}
#endif

	return value.m_result;
}

void RdTscStopwatch::calculateTscOverhead()
{
	__int64 times[10];
	for (int i = 0; i < 10; ++i)
	{
		times[i] = calculateSingleTimeOverhead();
	}

	m_associatedOverhead = 0;

	for (int i = 0; i < 10; ++i)
	{
		m_associatedOverhead += times[i];
	}

	m_associatedOverhead /= 10;
}

int64_t RdTscStopwatch::getOverhead()
{
	return m_associatedOverhead;
}

int64_t RdTscStopwatch::m_associatedOverhead = 0;
