#include "BufferObject.h"
#include <cassert>
#include "Rendering/Debug/RenderingDebugOutput.h"

namespace oplo
{

BufferObject::BufferObject() : m_id(0), m_pointer(0)
{}

BufferObject::~BufferObject()
{
	deallocate();
}

void BufferObject::create(const char* label /*= 0*/)
{
	if (!m_id)
	{
		glGenBuffers(1, &m_id);

		if (label)
		{
			oplo::ObjectLabel(GL_BUFFER, m_id, -1, label);
		}
		else
		{
			oplo::ObjectLabel(GL_BUFFER, m_id, -1, "Unnamed Buffer");
		}
	}
	else
	{
		oplo::DebugMessage(
			GL_DEBUG_SOURCE_APPLICATION, 
			GL_DEBUG_TYPE_OTHER,
			0,
			GL_DEBUG_SEVERITY_NOTIFICATION,
			"Trying to recreate already allocated buffer: %s", label ? label : "Unnamed Bufer");
	}
}

void BufferObject::allocate(
		unsigned bindingHint, 
		unsigned size, 
		unsigned flags,
		const void* data,
		bool immutable )
{
	if (!m_id)
	{
		oplo::DebugMessage(
			GL_DEBUG_SOURCE_APPLICATION,
			GL_DEBUG_TYPE_ERROR,
			0,
			GL_DEBUG_SEVERITY_NOTIFICATION,
			"Allocating buffer before it's created");
	}

	if (!immutable)
	{
		glNamedBufferDataEXT(m_id, size, data, flags);
	}
	else
	{
		glNamedBufferStorageEXT(m_id, size, data, flags);
	}
}

void BufferObject::setSubData(
	unsigned bindingHint,
	unsigned offset,
	unsigned size,
	const void* data
	)
{
	glNamedBufferSubDataEXT(m_id, offset, size, data);
}

void BufferObject::invalidate() const
{
	glInvalidateBufferData(m_id);
}

void BufferObject::deallocate()
{
	glDeleteBuffers(1, &m_id);
	m_id = 0;
}

void BufferObject::map(unsigned mapFlags)
{
	assert(!m_pointer);
	m_pointer = glMapNamedBufferEXT(m_id, mapFlags);
}

void BufferObject::unmap()
{
	assert(m_pointer);
	glUnmapNamedBufferEXT(m_id);
	m_pointer = 0;
}

unsigned BufferObject::getId() const
{
	return m_id;
}

}

