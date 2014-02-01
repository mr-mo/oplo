#include "LAS.h"

namespace oplo
{
	LASReader::LASReader() : m_buffer(0), m_bufferSize(0)
	{}

	bool LASReader::open(const char* fn, lasUint64 numberOfPointsToBuffer)
	{
		m_fileName = fn;
		m_fileStream.open(fn, std::ios::binary);

		if (!m_fileStream.good())
			return false;

		char buffer[227];
		char* bufferIterator = buffer;

		m_fileStream.read(bufferIterator, 227);
		memcpy(&m_header.m_fileSig, bufferIterator, 4); bufferIterator += 4;
		memcpy(&m_header.m_sourceId, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_encoding, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_projectID0, bufferIterator, 4); bufferIterator += 4;
		memcpy(&m_header.m_projectID1, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_projectID2, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_projectID3, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_majorVer, bufferIterator, 1); bufferIterator += 1;
		memcpy(&m_header.m_minorVer, bufferIterator, 1); bufferIterator += 1;

		if (m_header.m_majorVer != 1)
			printf("Unknown LAS major version\n");

		if (m_header.m_minorVer < 3)
			printf("No support for waveform data.\n");

		if (m_header.m_minorVer > 3)
			printf("Version 1.4, still beta support.\n");

		memcpy(&m_header.m_systemIdentifier, bufferIterator, 32); bufferIterator += 32;
		memcpy(&m_header.m_generatingSoftware, bufferIterator, 32); bufferIterator += 32;
		memcpy(&m_header.m_fileCreationDayOfYear, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_fileCreationYear, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_headerSize, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_offsetToPointData, bufferIterator, 4); bufferIterator += 4;
		memcpy(&m_header.m_numberOfVariableLengthRecords, bufferIterator, 4); bufferIterator += 4;
		memcpy(&m_header.m_pointDataFormatID, bufferIterator, 1); bufferIterator += 1;
		memcpy(&m_header.m_pointRecordLength, bufferIterator, 2); bufferIterator += 2;
		memcpy(&m_header.m_numberOfPointRecords, bufferIterator, 4); bufferIterator += 4;
		memcpy(&m_header.m_pointsByReturn, bufferIterator, 20); bufferIterator += 20;

		memcpy(&m_header.m_xScale, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_yScale, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_zScale, bufferIterator, 8); bufferIterator += 8;

		memcpy(&m_header.m_xOffset, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_yOffset, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_zOffset, bufferIterator, 8); bufferIterator += 8;

		memcpy(&m_header.m_maxX, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_minX, bufferIterator, 8); bufferIterator += 8;

		memcpy(&m_header.m_maxY, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_minY, bufferIterator, 8); bufferIterator += 8;

		memcpy(&m_header.m_maxZ, bufferIterator, 8); bufferIterator += 8;
		memcpy(&m_header.m_minZ, bufferIterator, 8); bufferIterator += 8;

		if (m_header.m_minorVer > 2)
		{
			m_fileStream.read(reinterpret_cast<char*>(&m_header.m_startOfWaveformData), 8);
		}

		if (m_header.m_minorVer > 3)
		{
			m_fileStream.read(reinterpret_cast<char*>(&m_header.m_startOfExtendedVariableLengthRecords), 8);
			m_fileStream.read(reinterpret_cast<char*>(&m_header.m_numberOfExtendedVariableLengthRecords), 4);
			m_fileStream.read(reinterpret_cast<char*>(&m_header.m_totalPointRecords14), 8);
			m_fileStream.read(reinterpret_cast<char*>(&m_header.m_totalPointsByReturn14), 8 * 15);
		}

		setNumberOfPointsToBuffer(numberOfPointsToBuffer);

		return true;
	}

	const LASHeader& LASReader::getHeader() const
	{
		return m_header;
	}

	lasUint64 LASReader::getTotalPoints()
	{
		if (m_header.m_minorVer == 4)
		{
			return m_header.m_totalPointRecords14;
		}

		return m_header.m_numberOfPointRecords;
	}

	void LASReader::setNumberOfPointsToBuffer(lasUint64 pointCount)
	{
		if (!m_fileStream.good())
		{
			return;
		}

		if (m_buffer)
		{
			delete[] m_buffer;
		}

		m_bufferSize = pointCount * m_header.m_pointRecordLength;
		m_buffer = new char[m_bufferSize];
		m_fileStream.seekg(m_header.m_offsetToPointData);

		m_fileStream.read(&m_buffer[0], m_bufferSize);
		m_bufferPointer = m_header.m_offsetToPointData;
	}

	void LASReader::close()
	{
		m_fileStream.close();

		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = 0;
			m_bufferSize = 0;
		}
	}

}
