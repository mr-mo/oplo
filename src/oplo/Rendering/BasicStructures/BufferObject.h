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

	void allocate(
			unsigned bindingHint, 
			unsigned size, 
			unsigned flags,
			const void* data = 0,
			bool immutable = false );

	void setSubData(
		unsigned bindingHint,
		unsigned offset,
		unsigned size,
		const void* data
		);

	void invalidate() const;

	void create(const char* label = 0);

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

	//todo: factor me out... this makes class a lot fatter than it needs to be
	void* m_pointer;

	unsigned m_id;
};

}

#endif
