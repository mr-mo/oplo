#include <Profiling/Timers/Chrono/ChronoStopwatch.h>
#include <Profiling/Timers/RdTsc/RdTscStopwatch.h>
#include <Profiling/Timers/RdTscp/RdTscpStopwatch.h>
#include <Profiling/Timers/QPC/QPCStopwatch.h>
#include <Profiling/CPUInformation.h>
#include <Containers/HashMap/HashMap.h>
#include <Math/Vector.h>
#include <iostream>
#include <unordered_map>

void printValues(const __int64* diff, const int loopCounter)
{
	__int64 minV = diff[0], maxV = diff[0];
	double avg = 0;

	for (int i = 1; i < loopCounter; ++i)
	{
		__int64 d = diff[i]; // -diff[i - 1];
		avg += d;
		minV = d < minV ? d : minV;
		maxV = d > maxV ? d : maxV;
	}

	avg /= loopCounter;
	double stdDev = 0;

	for (int i = 1; i < loopCounter; ++i)
	{
		__int64 d = diff[i]; // -diff[i - 1];
		stdDev += (d - avg) * (d - avg);
	}

	stdDev /= loopCounter;
	stdDev = sqrt(stdDev);

	printf("Test: %f avg time, %f sigma, %I64i min %I64i max\n", avg, stdDev, minV, maxV);

}


#include <type_traits>

template<int idx, typename Func>
void approximateResolution(Func f, const int loopCounter = 1000000)
{
	typedef typename std::result_of_t<Func()> dataType;
	dataType* storage = new dataType[loopCounter];
	__int64* diffs = new __int64[loopCounter];

	RdTscStopwatch tsc;

	__int64 overhead = std::numeric_limits<__int64>::max();

	tsc.start();
	for (int i = 0; i < loopCounter; ++i)
	{

		diffs[i] = tsc.runningDiff();
		tsc.serialize();
		//client code

		storage[i] = i;

		tsc.serialize();
		diffs[i] = tsc.runningDiff() - diffs[i]; // -RdTscStopwatch::getOverhead();
		overhead = overhead < diffs[i] ? overhead : diffs[i];
	}

	for (int i = 0; i < loopCounter; ++i)
	{
		diffs[i] = tsc.runningDiff();
		tsc.serialize();

		storage[i] = f();

		tsc.serialize();
		diffs[i] = tsc.runningDiff() - diffs[i];
	}

	for (int i = 0; i < loopCounter; ++i)
	{
		diffs[i] -= overhead;
	}

	std::cout << "Overhead: " << overhead << std::endl;

	tsc.stop();

	int istart = 0;

	int totalChanges = 0;
	__int64 change = 0;
	__int64 averageChange = 0;
	for (int i = 0; i < loopCounter; ++i)
	{
		if (storage[i] != storage[istart])
		{
			istart = i;
			++totalChanges;
		}

		change += diffs[i];
	}

	printValues(diffs, loopCounter);
	std::cout << "Average ticks per change: " << change / (double)totalChanges << std::endl;

	delete[] diffs;
	delete[] storage;
}

template<typename T >
struct Functor
{
	const T& m_v;

	Functor(const T& v) : m_v(v)
	{}

	inline typename T::ReturnType operator()() const
	{
		return m_v.poll();
	}
};


int main()
{
	CPUInformation info;
	info.setup();
	info.printInfo();

	SystemTimeWatch systemWatch;
	HighResWatch highResWatch;
	SteadyWatch steadyWatch;
	RdTscStopwatch rdTsc;
	QPCStopwatch qpc;
	RdTscpStopwatch rdTscp;


	static const int totalIterations = 500000;

	//unsigned* testArray0 = new unsigned[totalIterations];
	//unsigned* testArray1 = new unsigned[totalIterations];
	//bool* shouldTest = new bool[totalIterations];
	//unsigned numIgnored = 0;

	//for (int i = 0; i < totalIterations; ++i)
	//{
	//	testArray0[i] = rand() % totalIterations;
	//	testArray1[i] = rand() % totalIterations;
	//}

	//DenseHashMap<unsigned, unsigned> hashMap(256);
	//std::cout << "MapSize: " << sizeof(hashMap) << std::endl;

	//rdTsc.start();
	//for (int i = 0; i < totalIterations; ++i)
	//{
	//	shouldTest[i] = hashMap.insert(testArray0[i], testArray1[i]);
	//}
	//rdTsc.stop();

	//std::cout << "Time insert: " << rdTsc.diff() << std::endl;

	//rdTsc.start();
	//for (int i = 0; i < totalIterations; ++i)
	//{
	//	if (shouldTest[i])
	//	{
	//		DenseHashMap<unsigned, unsigned>::Iterator itr = hashMap.find(testArray0[i]);
	//		if (hashMap.value(itr) != testArray1[i])
	//		{
	//			printf("Error!");
	//		}
	//	}
	//}
	//rdTsc.stop();
	//std::cout << "Time find: " << rdTsc.diff() << std::endl;

	//std::unordered_map<unsigned, unsigned> uhashMap;
	//std::cout << "MapSize: " << sizeof(uhashMap) << std::endl;

	//rdTsc.start();
	//for (int i = 0; i < totalIterations; ++i)
	//{
	//	uhashMap.insert(std::make_pair(testArray0[i], testArray1[i]));
	//}
	//rdTsc.stop();
	//std::cout << "Time uinsert: " << rdTsc.diff() << std::endl;

	//rdTsc.stop();
	//for (int i = 0; i < totalIterations; ++i)
	//{
	//	if (shouldTest[i])
	//	{
	//		std::unordered_map<unsigned, unsigned>::iterator itr = uhashMap.find(testArray0[i]);
	//		if (itr->second != testArray1[i])
	//		{
	//			printf("Error!");
	//		}
	//	}
	//}
	//rdTsc.stop();
	//std::cout << "Time find: " << rdTsc.diff() << std::endl;

	LARGE_INTEGER li;
	LARGE_INTEGER fcount;
	QueryPerformanceFrequency(&fcount);

	QueryPerformanceCounter(&li);
	long long t = _Xtime_get_ticks();
	__int64 tsc = __rdtsc();

	__int64 test = 2530000000 / fcount.QuadPart;

	__int64 data0 = rdTsc.poll();
	__int64 data1 = rdTsc.poll();

	//runLoop<0>([&rdTsc]() { rdTsc.poll(); });
	//runLoop<1>([&systemWatch](){ systemWatch.poll(); });
	//runLoop<2>([&highResWatch](){ highResWatch.poll(); });
	//runLoop<3>([&steadyWatch](){ steadyWatch.poll(); });
	//runLoop<4>([&li](){ QueryPerformanceCounter(&li); });
	//runLoop<5>([](){ _Xtime_get_ticks(); });

	FILETIME ft;

	approximateResolution<0>([&rdTsc]() { 
		return rdTsc.poll(); 
	});

	approximateResolution<1>([&systemWatch](){ 
		return systemWatch.poll(); 
	});
	
	approximateResolution<2>([&highResWatch](){ 
		return highResWatch.poll(); 
	});
	
	approximateResolution<3>([&steadyWatch](){ 
		return steadyWatch.poll(); 
	});
	
	approximateResolution<4>([&li]() { 
		QueryPerformanceCounter(&li); return li.QuadPart; 
	});
	
	approximateResolution<5>([]() { 
		return _Xtime_get_ticks(); 
	});
	
	approximateResolution<6>([&ft, &li]() {
		GetSystemTimeAsFileTime(&ft);
		li.HighPart = ft.dwHighDateTime;
		li.LowPart = ft.dwLowDateTime;
		return li.QuadPart;
	});

	approximateResolution<7>([&qpc]() {
		return qpc.poll();
	});

	approximateResolution<8>([&rdTscp]() {
		return rdTscp.poll();
	});


	std::cin.get();
	return -1;
}
