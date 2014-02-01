#ifndef LAS_STRUCTURES_H
#define LAS_STRUCTURES_H

namespace oplo
{

	typedef char lasChar;
	typedef unsigned char lasUChar;
	typedef short lasInt16;
	typedef unsigned short lasUint16;
	typedef int lasInt32;
	typedef unsigned int lasUint32;
	typedef double lasFloat64;
	typedef float lasFloat32;
	typedef unsigned long long lasUint64;

	struct LASHeader
	{
		lasChar m_fileSig[4];
		lasUint16 m_sourceId;
		lasUint16 m_encoding; //8
		lasUint32 m_projectID0;
		lasUint16 m_projectID1;
		lasUint16 m_projectID2; //16
		lasChar m_projectID3[8]; //24
		lasChar m_majorVer; // 25
		lasChar m_minorVer; // 26
		lasChar m_systemIdentifier[32]; //58
		lasChar m_generatingSoftware[32]; //90
		lasUint16 m_fileCreationDayOfYear; //92
		lasUint16 m_fileCreationYear; //94
		lasUint16 m_headerSize; //96
		lasUint32 m_offsetToPointData; //100
		lasUint32 m_numberOfVariableLengthRecords; //104
		lasChar m_pointDataFormatID; //105
		lasUint16 m_pointRecordLength; //107
		lasUint32 m_numberOfPointRecords; //111 //Deprecated after 1.3
		lasUint32 m_pointsByReturn[5]; //131

		lasFloat64 m_xScale; //139
		lasFloat64 m_yScale; //147
		lasFloat64 m_zScale; //155

		lasFloat64 m_xOffset; //163
		lasFloat64 m_yOffset; //171
		lasFloat64 m_zOffset; //179

		lasFloat64 m_maxX; //187
		lasFloat64 m_minX; //195

		lasFloat64 m_maxY; //203
		lasFloat64 m_minY; //211

		lasFloat64 m_maxZ; //219
		lasFloat64 m_minZ; //227

		lasUint64 m_startOfWaveformData;

		lasUint64 m_startOfExtendedVariableLengthRecords;
		lasUint32 m_numberOfExtendedVariableLengthRecords;
		lasUint64 m_totalPointRecords14; //After 1.4
		lasUint64 m_totalPointsByReturn14[15];
	};

	struct LASVariableLengthRecord
	{
		lasUint16 m_reserved;
		lasChar m_userID[16];
		lasUint16 m_recordID;
		lasUint16 m_recordLengthAfterHeader;
		lasChar m_description[16];
	};

	struct LASPoint0
	{
		lasInt32 m_X;
		lasInt32 m_Y;
		lasInt32 m_Z;

		lasUint16 m_intensity;
		lasUChar m_flags;
		lasUChar m_classification;
		lasChar m_scanAngle;
		lasUChar m_userData;
		lasUint16 m_pointSourceID;
	};

	struct LASPoint1 : LASPoint0
	{
		lasFloat64 m_GPSTime;
	};

	struct LASPoint2 : LASPoint0
	{
		lasUint16 m_red;
		lasUint16 m_green;
		lasUint16 m_blue;
	};

	struct LASPoint3 : LASPoint0
	{
		lasFloat64 m_GPSTime;

		lasUint16 m_red;
		lasUint16 m_green;
		lasUint16 m_blue;
	};

}

#endif
