#ifndef OPLO_QUERY_OBJECT_H
#define OPLO_QUERY_OBJECT_H

#include "Rendering/oploGL.h"

namespace oplo
{

	template<unsigned QueryEnum>
	class GPUQuery
	{
	public:

		GPUQuery() :
			m_query(0),
			m_available(0),
			m_value(0)
		{}

		void init()
		{
			glGenQueries(1, &m_query);
		}

		void destroy()
		{
			glDeleteQueries(1, &m_query);
		}

		void start()
		{
			m_available = 0;
			m_value = GLuint64(-1);
			glBeginQuery(QueryEnum, m_query);
		}

		void stop()
		{
			glEndQuery(QueryEnum);
		}

		GLint available()
		{
			if (!m_available)
			{
				glGetQueryObjectiv(m_query, GL_QUERY_RESULT_AVAILABLE, &m_available);
			}

			return m_available;
		}

		GLuint64EXT value()
		{
			if (m_value == GLuint64EXT(-1))
			{
				glGetQueryObjectui64v(m_query, GL_QUERY_RESULT, &m_value);
			}

			return m_value;
		}


	private:

		GLuint64		m_value;
		GLint			m_available;
		unsigned		m_query;
	};

}

#endif
