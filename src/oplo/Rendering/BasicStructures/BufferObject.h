#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include "Rendering/oploGL.h"
#include <cassert>

namespace oplo
{

class BufferObject
{
public:

	BufferObject();

	~BufferObject();

	void setImmutable(bool immutable);

	bool isImmutable() const;

	void allocate(
			unsigned bindingHint, 
			unsigned dim, 
			unsigned size, 
			unsigned flags,
			const char* );

	void invalidate() const;

	void deallocate();

	template<typename T>
	T* getMappedPointer()
	{
		return reinterpret_cast<T*>(m_pointer);
	}

	void map(unsigned mapFlags);

	void unmap();

	unsigned getId() const;

private:

	bool m_immutable;

	void* m_pointer;

	unsigned m_id;
};

}

#endif
