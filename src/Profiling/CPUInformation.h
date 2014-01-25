#ifndef CPU_INFORMATION_H
#define CPU_INFORMATION_H

class CPUInformation
{
public:

	CPUInformation();

	void setup();

	void printInfo() const;

	int getModel() const;

	int getFamily() const;

	int getType() const;

	int getCacheSize() const;

	int getCacheLineSize() const;

	int getLogcalProcessors() const;

	bool invariantTSC() const;

	bool supportsRDTSCP() const;

	//int m_cores;
	//int m_maximumThreads;

private:

	char m_brand[0x40];
	char m_cpu[0x20];
	int m_model;
	int m_family;
	int m_type;
	int m_cacheSize;
	int m_cacheLineSize;
	int m_logicalProcessors;
	int m_cores;
	int m_maximumThreads;
	bool m_invariantTSC;
	bool m_supportsRDTSCP;
};


#endif
