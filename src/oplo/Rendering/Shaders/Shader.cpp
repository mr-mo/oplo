#include "Shader.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "Rendering/Debug/RenderingDebugOutput.h"

namespace oplo
{
	Shader::Shader(Target target, const char* label) : m_id(0), m_target(target), m_label(label)
	{}

	Shader::~Shader()
	{
		//must deallocate resources explicitly when we know we have a context
		assert(!m_id && "Must delete resources explicitly.");
	}

	unsigned Shader::getId() const
	{
		return m_id;
	}

	void Shader::destroy()
	{
		if (m_id)
		{
			glDeleteShader(m_id);
			m_id = 0;
		}
	}

	void Shader::create()
	{
		if (m_id)
		{
			oplo::DebugMessage(
					GL_DEBUG_SOURCE_APPLICATION, 
					GL_DEBUG_TYPE_ERROR, 
					m_id, 
					GL_DEBUG_SEVERITY_MEDIUM, 
					"%s already created", m_label.c_str());
			return;
		}

		m_id = glCreateShader(m_target);

		oplo::ObjectLabel(GL_SHADER, m_id, -1, m_label.c_str());
	}

	Shader::Target Shader::getTarget() const
	{
		return m_target;
	}

	void Shader::addFile(const std::string& fn)
	{
		m_files.push_back(fn);
	}

	void Shader::addInline(const std::string& fn)
	{
		m_inlined.push_back(fn);
	}

	void Shader::clearInlined()
	{
		m_inlined.swap(std::vector<std::string>());
	}

	void Shader::compile()
	{
		if (!m_id)
		{
			oplo::DebugMessage(
					GL_DEBUG_SOURCE_APPLICATION, 
					GL_DEBUG_TYPE_ERROR, 
					0, 
					GL_DEBUG_SEVERITY_MEDIUM, 
					"Compiling %s before created", m_label.c_str());
			return;
		}

		std::vector< std::string > source;

		bool success = true;

		std::size_t failed = 0;

		for (std::size_t t = 0; t < m_inlined.size(); ++t)
		{
			source.push_back(m_inlined[t]);
			source.push_back("\n");
		}

		for (std::size_t t = 0; t < m_files.size(); ++t)
		{
			source.push_back(std::string());
			success = loadFile(m_files[t], source.back());

			if (!success)
			{
				oplo::DebugMessage(
					GL_DEBUG_SOURCE_APPLICATION,
					GL_DEBUG_TYPE_ERROR,
					0,
					GL_DEBUG_SEVERITY_MEDIUM,
					"File %s filed to load", m_files[t].c_str());
			}
		}

		const char** f = new const char*[source.size()];

		for (unsigned i = 0; i < source.size(); ++i)
		{
			f[i] = source[i].c_str();
		}

		glShaderSource(m_id, static_cast<GLsizei>(source.size()), f, 0);

		glCompileShader(m_id);

		checkLog();

		delete[] f;
	}

	bool Shader::loadFile(const std::string& filename, std::string& result)
	{
		std::ifstream file(filename.c_str());

		if (!file)
		{
			return false;
		}

		result.clear();

		std::string line;

		while (getline(file, line))
		{
			result += line;
			result += '\n';
		}

		file.close();

		return true;
	}

	void Shader::checkLog()
	{
		int logLength = 0;

		//glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, m_id, -1, m_label.c_str());
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 1)
		{
			char *log = new char[logLength];
			glGetShaderInfoLog(m_id, logLength, &logLength, log);

			oplo::DebugMessageS(
				GL_DEBUG_SOURCE_APPLICATION,
				GL_DEBUG_TYPE_OTHER,
				0,
				GL_DEBUG_SEVERITY_NOTIFICATION,
				log);

			delete[] log;
		}

		glGetShaderiv(m_id, GL_COMPILE_STATUS, &logLength);
		
		//glPopDebugGroup();

	}
}
