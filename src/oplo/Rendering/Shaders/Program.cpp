#include "Rendering/Shaders/Program.h"
#include "Rendering/Shaders/Uniform.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Debug/RenderingDebugOutput.h"
#include <cassert>

namespace oplo
{

	Program::Program(const char* name) :
		m_id(0),
		m_uniforms(0),
		m_inputs(0),
		m_outputs(0),
		m_maxInputs(0),
		m_maxOutputs(0),
		m_maxUniform(0),
		m_name(name)
	{ }

	Program::~Program()
	{
		assert(!m_id && "Must delete resources explicitly.");
	}

	void Program::destroy()
	{
		for (auto s : m_shaders)
		{
			s->destroy();
		}

		delete[] m_inputs;
		delete[] m_uniforms;
		delete[] m_outputs;

		m_inputs = 0;
		m_uniforms = 0;
		m_outputs = 0;

		if (m_id)
		{
			glDeleteProgram(m_id);
			m_id = 0;
		}

		m_shaders.swap(std::vector<std::shared_ptr<Shader>>());
	}

	void Program::compile()
	{
		for (auto s : m_shaders)
		{
			s->compile();
		}

		link();

		compileEvent();
	}

	void Program::compileEvent()
	{}

	void Program::setUniform(const std::string& uniform, const char* value, unsigned sz)
	{
		int idx = getUniformLoc(uniform);

		if (idx > -1)
			setUniform(idx, value, sz);
	}

	void Program::setUniform(int idx, const char* value, unsigned sz)
	{
		m_uniforms[idx].update(value, sz);
	}

	int Program::getUniformLoc(const std::string& uni_name) const
	{
		for (int i = 0; i < m_maxUniform; ++i)
		{
			if (m_uniforms[i].getName() == uni_name)
				return i;
		}

		return -1;
	}

	int Program::getInputLoc(const std::string&  attrib_name) const
	{
		for (int i = 0; i < m_maxInputs; ++i)
		{
			if (m_inputs[i].m_name == attrib_name)
				return m_inputs[i].m_location;
		}

		return -1;
	}

	int Program::getOutputLoc(const std::string&  attrib_name) const
	{
		for (int i = 0; i < m_maxOutputs; ++i)
		{
			if (m_outputs[i].m_name == attrib_name)
				return m_outputs[i].m_location;
		}

		return -1;
	}

	void Program::addShader(std::shared_ptr<Shader> shader)
	{
		m_shaders.push_back(shader);
	}

	//void Program::BindFragDataLoc(const char* data_name, unsigned loc)
	//{
	//	glBindFragDataLocation(program_, loc, data_name);
	//	SetFragDataLoc(data_name, loc);
	//}

	bool Program::link()
	{
		if (m_id)
		{
			glDeleteProgram(m_id);
			m_id = 0;
		}

		m_id = glCreateProgram();

		oplo::ObjectLabel(GL_PROGRAM, m_id, -1, m_name.c_str());

		for (auto s : m_shaders)
		{
			if (s->getId())
			{
				glAttachShader(m_id, s->getId());
			}

		}

		//if (!transform_feedbacks_.empty())
		//{
		//	const char** xform_feed = new const char*[transform_feedbacks_.size()];

		//	for (unsigned i = 0; i < transform_feedbacks_.size(); ++i)
		//	{
		//		xform_feed[i] = transform_feedbacks_[i].c_str();
		//	}

		//	glTransformFeedbackVaryings(program_, transform_feedbacks_.size(), xform_feed, transform_feedback_type_);

		//	delete[] xform_feed;
		//}

		glLinkProgram(m_id);

		GLint charsWritten, infoLogLength;

		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 1)
		{
			char * log = new char[infoLogLength];
			glGetProgramInfoLog(m_id, infoLogLength, &charsWritten, log);

			oplo::DebugMessageS(
				GL_DEBUG_SOURCE_APPLICATION,
				GL_DEBUG_TYPE_OTHER,
				0,
				GL_DEBUG_SEVERITY_NOTIFICATION,
				log);

			delete[] log;
		}

		GLint success = GL_FALSE;
		glGetProgramiv(m_id, GL_LINK_STATUS, &success);

		fillUniformData();
		fillIOData(GL_PROGRAM_INPUT, m_maxInputs, m_inputs);
		fillIOData(GL_PROGRAM_OUTPUT, m_maxOutputs, m_outputs);

		//if( success == GL_FALSE )
		//{
		//	glDeleteProgram( program_ );
		//	return false;
		//}

		//int uniformMaxLength;
		//glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &uniform_count_);
		//glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

		//GLint count = -1;
		//GLenum type = 0;
		//GLchar* uniformName = new GLchar[uniformMaxLength];

		//for (GLint i = 0; i < uniform_count_; ++i)
		//{
		//	GLsizei length;

		//	glGetActiveUniformName(program_, i, uniformMaxLength, &length, uniformName);

		//	int loc = glGetUniformLocation(program_, uniformName);

		//	uniformVector_.push_back(std::shared_ptr< UniformValue >(new UniformValue(uniformName, loc)));
		//	mUniforms_.insert(std::make_pair(uniformName, uniformVector_.back()));
		//}

		//delete[] uniformName;

		//int attributeMaxLength;
		//glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &attribute_count_);
		//glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength);

		//GLchar* attributeName = new GLchar[attributeMaxLength];

		//for (GLint i = 0; i < attribute_count_; ++i)
		//{
		//	GLsizei length;
		//	GLint size;
		//	GLenum type;

		//	glGetActiveAttrib(program_, i, attributeMaxLength, &length, &size, &type, attributeName);

		//	int loc = glGetAttribLocation(program_, attributeName);

		//	attributeLocations_.insert(std::make_pair(attributeName, loc));
		//}

		//delete[] attributeName;

		//Bind();

		//for (SamplerLocationMap::iterator itr = samplerLocations_.begin(); itr != samplerLocations_.end(); ++itr)
		//{
		//	int mloc = glGetUniformLocation(program_, itr->first.c_str());

		//	if (mloc != -1)
		//		glUniform1i(mloc, itr->second);
		//}

		//for (FragDataLocationMap::iterator itr = fragmentDataLocation_.begin(); itr != fragmentDataLocation_.end(); ++itr)
		//{
		//	glBindFragDataLocation(program_, itr->second, itr->first.c_str());
		//}

		//Unbind();

		return true;

	}

	//void Program::AddTransformFeedback(const char* strFeedback)
	//{
	//	transform_feedbacks_.push_back(strFeedback);
	//}
	//
	//void Program::ClearTransformFeedbacks()
	//{
	//	transform_feedbacks_.swap(std::vector<std::string>());
	//}
	//
	//void Program::SetTransformFeedbackType(unsigned uEnum)
	//{
	//	transform_feedback_type_ = uEnum;
	//}


	void Program::fillUniformData()
	{
		if (m_uniforms)
		{
			delete[] m_uniforms;
			m_uniforms = 0;
			m_maxUniform = 0;
		}

		int stringLength = 0;
		int numUnis;
		glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUnis);
		glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_MAX_NAME_LENGTH, &stringLength);
		char* uniformName = new char[stringLength];
		GLenum things[] = { GL_LOCATION, GL_TYPE, GL_ARRAY_SIZE };
		GLint params[3] = { 0 };
		GLsizei tmpL;

		for (int i = 0; i < numUnis; ++i)
		{
			glGetProgramResourceiv(m_id, GL_UNIFORM, i, 1, things, 1, 0, params);
			m_maxUniform = params[0] > m_maxUniform ? params[0] : m_maxUniform;
		}
		m_maxUniform += 1;
		m_uniforms = new UniformValue[m_maxUniform];

		for (int i = 0; i < numUnis; ++i)
		{
			glGetProgramResourceName(m_id, GL_UNIFORM, i, stringLength, &tmpL, uniformName);

			glGetProgramResourceiv(m_id, GL_UNIFORM, i, sizeof(things) / sizeof(GLenum), things, sizeof(params) / sizeof(GLint), 0, params);

			m_uniforms[params[0]].setName(uniformName);
			m_uniforms[params[0]].setLocation(params[0]);
			m_uniforms[params[0]].setType(params[1]);
		}

		delete[] uniformName;
	}

	void Program::fillUniformBlocks()
	{}

	void Program::fillIOData(GLenum target, GLint& tot, Attribute*& ar)
	{
		if (ar)
		{
			delete[] ar;
			ar = 0;
			tot = 0;
		}

		int stringLength = 0;
		glGetProgramInterfaceiv(m_id, target, GL_ACTIVE_RESOURCES, &tot);
		glGetProgramInterfaceiv(m_id, target, GL_MAX_NAME_LENGTH, &stringLength);
		char* attributeName = new char[stringLength];
		GLenum things[] = { GL_LOCATION, GL_TYPE, GL_ARRAY_SIZE };
		GLint params[3] = { 0 };
		GLsizei tmpL;

		ar = new Attribute[tot];

		for (int i = 0; i < tot; ++i)
		{
			glGetProgramResourceName(m_id, target, i, stringLength, &tmpL, attributeName);

			glGetProgramResourceiv(m_id, target, i, sizeof(things) / sizeof(GLenum), things, sizeof(params) / sizeof(GLint), 0, params);

			ar[i].m_location = params[0];
			ar[i].m_type = params[1];
			ar[i].m_name = attributeName;
		}

		delete[] attributeName;
	}


	int Program::getTotalUniforms() const
	{
		return m_maxUniform;
	}

	const std::string& Program::getUniformName(int index) const
	{
		return m_uniforms[index].getName();
	}

	unsigned Program::getId() const
	{
		return m_id;
	}

	void Program::bind() const
	{
		glUseProgram(m_id);

		for (int i = 0; i < m_maxUniform; ++i)
		{
			m_uniforms[i].apply();
		}
	}

	void Program::unbind() const
	{
		glUseProgram(0);
	}

}
