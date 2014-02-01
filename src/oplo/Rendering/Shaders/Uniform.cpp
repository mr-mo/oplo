#include "Rendering/Shaders/Uniform.h"

namespace oplo
{
	UniformValue::UniformValue() :
		m_location(-1)
		, m_shouldUpdate(false)
		, m_type(0)
	{ }

	UniformValue::UniformValue(const std::string& name) :
		m_location(-1)
		, m_name(name)
		, m_shouldUpdate(false)
		, m_type(0)
	{ }

	UniformValue::UniformValue(const std::string& name, int loc, GLint type) :
		m_location(loc)
		, m_name(name)
		, m_shouldUpdate(false)
		, m_type(type)
	{ }

	UniformValue::UniformValue(const UniformValue& rhs) :
		m_location(rhs.m_location)
		, m_value(rhs.m_value)
		, m_shouldUpdate(rhs.m_shouldUpdate)
		, m_type(rhs.m_type)
		, m_name(rhs.m_name)
	{}

	void UniformValue::forceNeedsUpdate()
	{
		m_shouldUpdate = true;
	}

	bool UniformValue::update(const char* val, unsigned sz)
	{
		if (m_location == -1)
			return false;

		if (sz != m_value.size())
		{
			m_value.resize(sz);
			m_shouldUpdate = true;
			memcpy(&m_value[0], val, sz);
			return true;
		}

		for (unsigned i = 0; i < sz; ++i)
		{
			if (m_value[i] != val[i])
			{
				m_shouldUpdate = true;
			}

			m_value[i] = val[i];
		}

		return m_shouldUpdate;
	}

	void UniformValue::apply() const
	{
		if (!m_shouldUpdate)
			return;

		switch (m_type)
		{
		case GL_FLOAT:
			glUniform1fv(m_location, GLsizei(m_value.size() / (sizeof(float))), reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 2)), reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 3)), reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 4)), reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;

		case GL_INT:
			glUniform1iv(m_location, GLsizei(m_value.size() / (sizeof(int))), reinterpret_cast<const GLint*>(&m_value[0]));
			break;
		case GL_INT_VEC2:
			glUniform2iv(m_location, GLsizei(m_value.size() / (sizeof(int)* 2)), reinterpret_cast<const GLint*>(&m_value[0]));
			break;
		case GL_INT_VEC3:
			glUniform3iv(m_location, GLsizei(m_value.size() / (sizeof(int)* 3)), reinterpret_cast<const GLint*>(&m_value[0]));
			break;
		case GL_INT_VEC4:
			glUniform4iv(m_location, GLsizei(m_value.size() / (sizeof(int)* 4)), reinterpret_cast<const GLint*>(&m_value[0]));
			break;

		case GL_UNSIGNED_INT:
			glUniform1uiv(m_location, GLsizei(m_value.size() / (sizeof(unsigned))), reinterpret_cast<const GLuint*>(&m_value[0]));
			break;
		case GL_UNSIGNED_INT_VEC2:
			glUniform2uiv(m_location, GLsizei(m_value.size() / (sizeof(unsigned)* 2)), reinterpret_cast<const GLuint*>(&m_value[0]));
			break;
		case GL_UNSIGNED_INT_VEC3:
			glUniform3uiv(m_location, GLsizei(m_value.size() / (sizeof(unsigned)* 3)), reinterpret_cast<const GLuint*>(&m_value[0]));
			break;
		case GL_UNSIGNED_INT_VEC4:
			glUniform4uiv(m_location, GLsizei(m_value.size() / (sizeof(unsigned)* 4)), reinterpret_cast<const GLuint*>(&m_value[0]));
			break;

		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 4)), GL_FALSE, reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 9)), GL_FALSE, reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(m_location, GLsizei(m_value.size() / (sizeof(float)* 16)), GL_FALSE, reinterpret_cast<const GLfloat*>(&m_value[0]));
			break;

		}

		m_shouldUpdate = false;
	}

	void UniformValue::setType(GLint type)
	{
		m_type = type;
	}

	void UniformValue::setLocation(int loc)
	{
		m_location = loc;
	}

	void UniformValue::setName(const char* name)
	{
		m_name = name;
	}

	bool UniformValue::isValid() const
	{
		return m_location != -1;
	}

	const std::string& UniformValue::getName() const
	{
		return m_name;
	}

	int UniformValue::getLocation() const
	{
		return m_location;
	}

}
