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


#ifndef OPLO_RDTSC_TIMER_H
#define OPLO_RDTSC_TIMER_H

#include <stdint.h>

//Strictly speaking not a timer
//We do not try to associate ticks with time
//Rather just use the ticks as a performance metric over a frame
//To show inclusive/exclusive timings
class RdTscStopwatch
{
public:

	typedef int64_t ReturnType;

	RdTscStopwatch();

	inline void start();

	inline void stop();

	inline int64_t runningDiff() const;

	inline int64_t diff() const;

	inline static int64_t poll();

	inline static void serialize(); 

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

#include <Profiling/Timers/RdTsc/RdTscStopwatch.inl>


#endif
