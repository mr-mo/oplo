#include "Rendering/BasicStructures/VAO.h"
#include "Rendering/BasicStructures/BufferObject.h"
#include "Rendering\oploGL.h"

namespace oplo
{

	VAO::VAO() : m_vao(0)
	{}

	VAO::~VAO()
	{
		destroy();
	}

	void VAO::destroy()
	{
		if (m_vao)
		{
			glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
	}

	void VAO::addBufferDescriptor(std::shared_ptr<BufferObject> obj, const BufferDescriptor& desc)
	{
		m_buffers.push_back(BufferDesc(obj, desc));
	}

	void VAO::create()
	{
		if (m_vao)
		{
			destroy();
		}

		glGenVertexArrays(1, &m_vao);

		bind();

		for (int i = 0; i < m_buffers.size(); ++i)
		{
			glBindBuffer(m_buffers[i].m_desc.m_bufferType, m_buffers[i].m_obj->getId());
			for (int j = 0; j < m_buffers[i].m_desc.m_attributes.size(); ++j)
			{
				glEnableVertexAttribArray(m_buffers[i].m_desc.m_attributes[j].m_index);
				glVertexAttribPointer(
					m_buffers[i].m_desc.m_attributes[j].m_index,
					m_buffers[i].m_desc.m_attributes[j].m_numComponents,
					m_buffers[i].m_desc.m_attributes[j].m_type,
					GL_FALSE,
					m_buffers[i].m_desc.m_attributes[j].m_interleave,
					(char*)0 + m_buffers[i].m_desc.m_attributes[j].m_offset);
			}
		}

		unbind();

		//It seems like state dangles after this...
		for (int i = 0; i < m_buffers.size(); ++i)
		{
			glBindBuffer(m_buffers[i].m_desc.m_bufferType, 0);
			for (int j = 0; j < m_buffers[i].m_desc.m_attributes.size(); ++j)
			{
				glDisableVertexAttribArray(m_buffers[i].m_desc.m_attributes[j].m_index);
			}
		}
	}

	void VAO::bind() const
	{
		glBindVertexArray(m_vao);
	}

	void VAO::unbind() const
	{
		glBindVertexArray(0);
	}

}
