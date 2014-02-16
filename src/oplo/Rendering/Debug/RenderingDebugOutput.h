#ifndef RENDERING_DEBUG_OUTPUT_H
#define RENDERING_DEBUG_OUTPUT_H

#include <mutex>
#include <sstream>
#include <vector>
#include <iostream>

#include "Rendering/oploGL.h"

namespace oplo
{

	class RenderingDebugOutput
	{
	public:

		RenderingDebugOutput(std::function<void(const std::string&)> f = [](const std::string& str) { std::cout << str << std::endl; });

		~RenderingDebugOutput();

		void dumpOutput();

		void enable() const;

		void disable() const;

		void enableMultithreading();

		void disableMultithreading();

		void addToQueue(
			GLenum source​,
			GLenum type​,
			GLuint id​,
			GLenum severity​,
			GLsizei length​,
			const GLchar* message​);

		const char* sourceToString(GLenum e) const;

		const char* typeToString(GLenum e) const;

		const char* severityToString(GLenum e) const;

	private:

		std::mutex m_mutex;

		GLenum m_minimumSeverityLevel;

		std::vector<std::string> m_log;

		std::function<void(const std::string&)> m_func;

		bool m_multithreaded;
	};

	//TODO: Custom gl calls...
	void ObjectLabel(GLenum identifier, GLuint name, GLsizei size, const char* label);
		
	void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, const char* fmt, ...);

	void DebugMessageS(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);

	void GLAPIENTRY PrimaryDebugFunc(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		void* userParam);
}

#endif
