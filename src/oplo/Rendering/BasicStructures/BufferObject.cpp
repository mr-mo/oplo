#include "BufferObject.h"
#include <cassert>
#include "Rendering/Debug/RenderingDebugOutput.h"

namespace opMo
{

BufferObject::BufferObject() : m_immutable(false), m_id(0), m_pointer(0)
{}

BufferObject::~BufferObject()
{
	assert(!m_id);
}

void BufferObject::setImmutable(bool immutable)
{
	m_immutable = immutable;
}

void BufferObject::allocate(
		unsigned bindingHint, 
		unsigned dim, 
		unsigned size, 
		unsigned flags,
		const char* label )
{
	if (!m_immutable)
	{
		deallocate();
	}

	if (!m_id)
	{
		glGenBuffers(1, &m_id);
		
		if (label)
		{
			opMo::ObjectLabel(GL_BUFFER, m_id, -1, label);
		}
		else
		{
			opMo::ObjectLabel(GL_BUFFER, m_id, -1, "Unnamed Buffer");
		}
	}

	if (!m_immutable)
	{
		glNamedBufferDataEXT(m_id, dim * size, 0, flags);
	}
	else
	{
		glNamedBufferStorageEXT(m_id, dim * size, 0, flags);
	}
}

bool BufferObject::isImmutable() const
{
	return m_immutable;
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

