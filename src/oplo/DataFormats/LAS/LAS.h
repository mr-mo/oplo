#ifndef OPLO_LAS_H
#define OPLO_LAS_H

#include <string>
#include <fstream>

#include "LASStructures.h"

//Simple LAS reader
//No support for waveforms
//Marginal support for v1.4
//Tested on earlier versions

namespace oplo
{

	class LASReader
	{
	public:

		LASReader();

		//parses header
		bool open(const char* file, lasUint64 numberOfPointsToBuffer);

		const LASHeader& getHeader() const;

		lasUint64 getTotalPoints();

		template< typename pointType >
		const pointType* getPoint(lasUint64 i)
		{
			lasUint64 fileOffset = m_header.m_offsetToPointData + i * m_header.m_pointRecordLength;

			if (fileOffset < m_bufferPointer || (fileOffset - m_bufferPointer) >= m_bufferSize)
			{
				m_fileStream.seekg(fileOffset);
				m_fileStream.read(&m_buffer[0], m_bufferSize);
				m_bufferPointer = fileOffset;
			}

			return reinterpret_cast<pointType*>(&m_buffer[fileOffset - m_bufferPointer]);
		}

		void close();

	private:

		void setNumberOfPointsToBuffer(lasUint64 pointCount);

	private:

		lasUint64 m_bufferSize;
		lasUint64 m_bufferPointer;
		char* m_buffer;

		std::string		m_fileName;
		std::ifstream	m_fileStream;

		LASHeader m_header;
	};

}

#endif
