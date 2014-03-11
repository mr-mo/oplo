#ifndef OPLO_VAO_H
#define OPLO_VAO_H

#include <vector>
#include <memory>

#include "BufferDescriptor.h"

namespace oplo
{
	class BufferObject;
	class VAO
	{
	public:

		VAO();

		~VAO();

		void destroy();

		void addBufferDescriptor(std::shared_ptr<BufferObject> obj, const BufferDescriptor& desc);

		void create();

		void bind() const;

		void unbind() const;

	private:

		struct BufferDesc
		{
			BufferDesc(std::shared_ptr<BufferObject> obj, const BufferDescriptor& desc) :
				m_obj(obj), 
				m_desc(desc)
			{}

			std::shared_ptr<BufferObject> m_obj;
			BufferDescriptor m_desc;
		};

		unsigned m_vao;

		std::vector<BufferDesc> m_buffers;
	};
}

#endif
