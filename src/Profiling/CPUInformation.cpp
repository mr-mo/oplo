#include "CPUInformation.h"
#include <iostream>
#include <intrin.h>

CPUInformation::CPUInformation() :
	m_model(0),
	m_family(0),
	m_type(0),
	m_cacheSize(0),
	m_cacheLineSize(0),
	m_logicalProcessors(0),
	m_cores(0),
	m_maximumThreads(0),
	m_invariantTSC(false),
	m_supportsRDTSCP(false)
{
	memset(m_brand, 0, sizeof(m_brand));
	memset(m_cpu, 0, sizeof(m_cpu));
}

void CPUInformation::setup()
{
	int cpuInfo[4] = { -1 };
	unsigned int numIds;

	__cpuid(cpuInfo, 0);
	numIds = cpuInfo[0];
	*(reinterpret_cast<int*>(m_cpu + 0)) = cpuInfo[1];
	*(reinterpret_cast<int*>(m_cpu + 4)) = cpuInfo[3];
	*(reinterpret_cast<int*>(m_cpu + 8)) = cpuInfo[2];

	__cpuid(cpuInfo, 1);
	m_model = (cpuInfo[0] >> 4) & 0xf;
	m_family = (cpuInfo[0] >> 8) & 0xf;
	m_type = (cpuInfo[0] >> 12) & 0x3;
	m_logicalProcessors = (cpuInfo[1] >> 16) & 0xff;

	__cpuid(cpuInfo, 0x80000000);
	numIds = cpuInfo[0];

	for (unsigned int i = 0x80000000; i <= numIds; ++i)
	{
		__cpuid(cpuInfo, i);

		switch (i)
		{
		case 0x80000001: //ECX[27]
			m_supportsRDTSCP = (cpuInfo[3] & (1 << 27)) != 0;
			break;
		case 0x80000002:
			memcpy(m_brand, cpuInfo, sizeof(cpuInfo));
			break;
		case 0x80000003:
			memcpy(m_brand + 16, cpuInfo, sizeof(cpuInfo));
			break;
		case 0x80000004:
			memcpy(m_brand + 32, cpuInfo, sizeof(cpuInfo));
			break;
		case 0x80000006:
			m_cacheLineSize = cpuInfo[2] & 0xff;
			m_cacheSize = (cpuInfo[2] >> 16) & 0xffff;
			break;
		case 0x80000007: //EDX[8]
			m_invariantTSC = (cpuInfo[3] & (1 << 8)) != 0;
			break;
		}
	}

	//for (int i = 0;; ++i)
	//{
	//	__cpuidex(cpuInfo, 0x4, i);

	//	if (!cpuInfo[0] & 0xf0)
	//		break;

	//	if (i == 0)
	//	{
	//	}
	//}
}


void CPUInformation::printInfo() const
{
	std::cout << m_brand << std::endl;
	std::cout << m_cpu << std::endl;
	std::cout << "Cache Size: " << m_cacheSize << std::endl;
	std::cout << "Cache Line Size: " << m_cacheLineSize << std::endl;
	std::cout << "Invariant TSC: " << m_invariantTSC << std::endl;
	std::cout << "RDTSCP: " << m_supportsRDTSCP << std::endl;
}

int CPUInformation::getModel() const 
{ 
	return m_model; 
}

int CPUInformation::getFamily() const 
{ 
	return m_family; 
}

int CPUInformation::getType() const 
{ 
	return m_type; 
}

int CPUInformation::getCacheSize() const 
{ 
	return m_cacheSize; 
}

int CPUInformation::getCacheLineSize() const 
{ 
	return m_cacheLineSize; 
}

int CPUInformation::getLogcalProcessors() const 
{ 
	return m_logicalProcessors; 
}

bool CPUInformation::invariantTSC() const 
{ 
	return m_invariantTSC; 
}

bool CPUInformation::supportsRDTSCP() const 
{ 
	return m_supportsRDTSCP; 
}
