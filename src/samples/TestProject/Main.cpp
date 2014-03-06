#include <Profiling/Timers/Chrono/ChronoStopwatch.h>
#include <Profiling/Timers/RdTsc/RdTscStopwatch.h>
#include <Profiling/Timers/RdTscp/RdTscpStopwatch.h>
#include <Profiling/Timers/QPC/QPCStopwatch.h>
#include <Profiling/Timers/Prof/ProfStopwatch.h>
#include <Profiling/CPUInformation.h>
#include <Containers/HashMap/HashMap.h>
#include <Math/Vector.h>
#include <iostream>
#include <unordered_map>

void printValues( volatile __int64* diff, const int loopCounter)
{
	__int64 minV = diff[0], maxV = diff[0];
	double avg = diff[0];

	for (int i = 1; i < loopCounter; ++i)
	{
		__int64 d = diff[i]; // -diff[i - 1];
		avg += d;
		minV = d < minV ? d : minV;
		maxV = d > maxV ? d : maxV;
	}

	avg /= loopCounter;
	double stdDev = 0;

	for (int i = 0; i < loopCounter; ++i)
	{
		__int64 d = diff[i]; // -diff[i - 1];
		stdDev += (d - avg) * (d - avg);
	}

	stdDev /= loopCounter;
	stdDev = sqrt(stdDev);

	std::cout << "\tAverage time taken to query: " << avg << std::endl;
	std::cout << "\tMinimum time taken to query: " << minV << std::endl;
	std::cout << "\tMaximum time taken to query: " << maxV << std::endl;
}


#include <type_traits>
#include <iomanip>

template<int idx, typename Func>
void approximateResolution(Func f, const int loopCounter = 5000000)
{
	typedef typename std::result_of_t<Func()> dataType;
	volatile dataType* storage = new dataType[loopCounter];
	volatile __int64* diffs = new __int64[loopCounter];

	ProfStopwatch tsc;
	oplo::QPCStopwatch qpc;
	//__int64 overhead = std::numeric_limits<__int64>::max();
	//tsc.start();
	//for (int i = 0; i < loopCounter; ++i)
	//{

	//	diffs[i] = tsc.runningDiff();
	//	storage[i] = i;
	//	diffs[i] = tsc.runningDiff() - diffs[i]; // -RdTscStopwatch::getOverhead();
	//	overhead = overhead < diffs[i] ? overhead : diffs[i];
	//}


	qpc.start();
	for (int i = 0; i < loopCounter; ++i)
	{
		tsc.start();
		storage[i] = f();
		tsc.stop();
		diffs[i] = tsc.diff(); // tsc.pollE();
	}
	qpc.stop();

	tsc.stop();

	int istart = 0;

	int totalChanges = 0;
	__int64 change = 0;
	__int64 averageChange = 0;
	__int64 relativeChange = 0;
	for (int i = 0; i < loopCounter; ++i)
	{
		if (storage[i] != storage[istart])
		{
			relativeChange += storage[i] - storage[istart];
			istart = i;
			++totalChanges;
		}

		change += diffs[i];
	}

	printValues(diffs, loopCounter);

	std::cout << "\tAverage ticks elapsed per change: " << change / (double)totalChanges << std::endl;
	std::cout << "\tTotal Changes: " << totalChanges << std::endl;
	std::cout << "\tTotal Time: " << qpc.diffMS() << std::endl;
	std::cout << "\tFunctional Resolution (NumberChanges/Time): " << std::setprecision(20) << totalChanges / ((qpc.diffMS()) / 1000) << std::endl;
	std::cout << "\tFunctional Accuracy (SumChanges/Time): " << std::setprecision(20) << relativeChange / ((qpc.diffMS()) / 1000) << std::endl;
	std::cout << std::endl << std::endl;


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
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	CPUInformation info;
	info.setup();
	info.printInfo();
	oplo::QPCStopwatch::initialize();

	SystemTimeWatch systemWatch;
	HighResWatch highResWatch;
	SteadyWatch steadyWatch;
	RdTscStopwatch rdTsc;
	oplo::QPCStopwatch qpc;
	RdTscpStopwatch rdTscp;
	ProfStopwatch intelSW;

	LARGE_INTEGER li;
	LARGE_INTEGER fcount;
	QueryPerformanceFrequency(&fcount);
	std::cout << "QPC Freq: " << fcount.QuadPart << std::endl << std::endl << std::endl;


	FILETIME ft;

	std::cout << "Intel Query:" << std::endl;
	approximateResolution<10>([&intelSW]() {
		return intelSW.poll();
	});

	std::cout << "QPC:" << std::endl;
	approximateResolution<7>([&qpc]() {
		return qpc.poll();
	});

	std::cout << "std::system_clock:" << std::endl;
	approximateResolution<1>([&systemWatch](){
		return systemWatch.poll();
	});

	std::cout << "std::high_resolution_clock:" << std::endl;
	approximateResolution<2>([&highResWatch](){
		return highResWatch.poll();
	});

	std::cout << "std::steady_clock:" << std::endl;
	approximateResolution<3>([&steadyWatch](){
		return steadyWatch.poll();
	});

	std::cout << "GetSystemTimeAsFileTime" << std::endl;
	approximateResolution<6>([&ft, &li]() {
		GetSystemTimeAsFileTime(&ft);
		li.HighPart = ft.dwHighDateTime;
		li.LowPart = ft.dwLowDateTime;
		return li.QuadPart;
	});


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


	//approximateResolution<8>([&rdTscp]() {
	//	return rdTscp.poll();
	//});

	SYSTEMTIME st;

	//approximateResolution<9>([&st, &ft, &li]() {
	//	GetSystemTime(&st);
	//	SystemTimeToFileTime(&st, &ft);
	//	li.HighPart = ft.dwHighDateTime;
	//	li.LowPart = ft.dwLowDateTime;
	//	return li.QuadPart;
	//});


	std::cin.get();
	return -1;
}
