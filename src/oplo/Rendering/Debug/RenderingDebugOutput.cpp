#include "RenderingDebugOutput.h"

namespace oplo
{

RenderingDebugOutput::RenderingDebugOutput(std::function<void(const std::string&)> f /*= [](const std::string& str) { std::cout << str << std::endl; }*/) :
	m_func(f),
	m_minimumSeverityLevel(GL_DEBUG_SEVERITY_NOTIFICATION),
	m_multithreaded(true)
{}

RenderingDebugOutput::~RenderingDebugOutput()
{}

void RenderingDebugOutput::dumpOutput()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0; i < m_log.size(); ++i)
	{
		m_func(m_log[i]);
	}

	std::vector<std::string>().swap(m_log);
}

void RenderingDebugOutput::enable() const
{
	glEnable(GL_DEBUG_OUTPUT);
}

void RenderingDebugOutput::disable() const
{
	glDisable(GL_DEBUG_OUTPUT);
}

void RenderingDebugOutput::enableMultithreading()
{
	glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	m_multithreaded = true;
}

void RenderingDebugOutput::disableMultithreading()
{
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	m_multithreaded = false;
}

void RenderingDebugOutput::addToQueue(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message)
{
	if (severity < m_minimumSeverityLevel)
		return;

	std::stringstream errorString;

	errorString << "Debug Message:" <<
		"\n\t Source: " << sourceToString(source) <<
		"\n\t Type: " << typeToString(type) <<
		"\n\t Severity: " << severityToString(severity) <<
		"\n\t ID: " << id <<
		"\n\t Message: " << message << "\n";

	if (m_multithreaded)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_log.push_back(std::move(errorString.str()));
	}
	else
	{
		m_func(errorString.str());
	}
}

const char* RenderingDebugOutput::sourceToString(GLenum e) const
{
	switch (e)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		return "Broken";
	}
}

const char* RenderingDebugOutput::typeToString(GLenum e) const
{
	switch (e)
	{
	case GL_DEBUG_TYPE_ERROR:
		return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behavior";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance";
	case GL_DEBUG_TYPE_MARKER:
		return "Marker";
	case GL_DEBUG_TYPE_PUSH_GROUP:
		return "Push Group";
	case GL_DEBUG_TYPE_POP_GROUP:
		return "Pop Group";
	case GL_DEBUG_TYPE_OTHER:
		return "Other";
	default:
		return "Broken";
	}
}

const char* RenderingDebugOutput::severityToString(GLenum e) const
{
	switch (e)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		return "High";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "Medium";
	case GL_DEBUG_SEVERITY_LOW:
		return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "Notification";
	default:
		return "Broken";
	}
}

void ObjectLabel(GLenum identifier, GLuint name, GLsizei size, const char* label)
{
	if (GLEW_KHR_debug)
	{
		glObjectLabel(identifier, name, size, label);
	}
}

void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, const char* fmt, ...)
{
	if (GLEW_KHR_debug)
	{
		char buf[256];
		va_list arglist;
		va_start(arglist, fmt);
		vsnprintf_s(buf, 256, fmt, arglist);
		va_end(arglist);

		glDebugMessageInsert(source, type, id, severity, -1, buf);
	}
}

void DebugMessageS(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg )
{
	if (GLEW_KHR_debug)
	{
		glDebugMessageInsert(source, type, id, severity, -1, msg);
	}
}


void  PrimaryDebugFunc(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* userParam)
{
	RenderingDebugOutput* debugger = reinterpret_cast<RenderingDebugOutput*>(userParam);
	debugger->addToQueue(source, type, id, severity, length, message);
}

}

