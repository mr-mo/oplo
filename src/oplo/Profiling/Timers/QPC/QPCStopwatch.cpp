#include "Profiling\Timers\QPC\QPCStopwatch.h"

namespace oplo
{

	void QPCStopwatch::initialize()
	{
		QueryPerformanceFrequency(&m_frequency);
	}

	LARGE_INTEGER QPCStopwatch::m_frequency = LARGE_INTEGER();

}
