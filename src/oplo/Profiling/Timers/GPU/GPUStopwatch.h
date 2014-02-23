#ifndef OPLO_GPU_STOPWATCH_H
#define OPLO_GPU_STOPWATCH_H

#include "Rendering/Queries/QueryObject.h"

namespace oplo
{

	class GPUStopwatch
	{
	public:

		typedef GLuint64 ReturnType;

		GPUStopwatch()
		{
		}

		~GPUStopwatch()
		{
			m_query.destroy();
		}

		inline void init()
		{
			m_query.init();
		}

		inline void destroy()
		{
			m_query.destroy();
		}

		inline void start()
		{
			m_query.start();
		}

		inline void stop()
		{
			m_query.stop();
		}

		inline ReturnType diff() const
		{
			return m_query.value();
		}

		inline double diffMS() const
		{
			return diff() / 1000000.0;
		}

	private:

		mutable GPUQuery<GL_TIME_ELAPSED> m_query;
	};

}

#endif
