#ifndef OPLO_BUFFER_DESCRIPTOR_H
#define OPLO_BUFFER_DESCRIPTOR_H

#include <vector>

namespace oplo
{
	class BufferDescriptor
	{
	public:

		struct AttributeData
		{
			AttributeData() : 
				m_index(-1), 
				m_type(-1), 
				m_interleave(-1), 
				m_offset(-1),
				m_numComponents(-1)
			{}

			AttributeData(int index, int numComponents, int type, int interleave, int offset) :
				m_index(index), 
				m_numComponents(numComponents), 
				m_type(type), 
				m_interleave(interleave), 
				m_offset(offset)
			{}

			int m_index;
			int m_type;
			int m_interleave;
			int m_offset;
			int m_numComponents;
		};

		BufferDescriptor() : m_bufferType(0)
		{}

		BufferDescriptor(unsigned bufferType) : m_bufferType(bufferType)
		{}

		BufferDescriptor(unsigned bufferType, std::initializer_list<AttributeData> data) : 
			m_bufferType(bufferType)
		{
			m_attributes.insert(m_attributes.end(), data.begin(), data.end());
		}

		unsigned m_bufferType;

		std::vector<AttributeData> m_attributes;

	};
}

#endif
