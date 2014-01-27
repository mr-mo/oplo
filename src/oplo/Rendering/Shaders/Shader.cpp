#include "Shader.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "Rendering/Debug/RenderingDebugOutput.h"

namespace oplo
{

	Shader::Shader() : id_(0), target_(UNKNOWN)
	{ }

	Shader::Shader(Target target, const char* label) : id_(0)
	{
		Create(target, label);
	}

	Shader::~Shader()
	{
		//we do not call destroy in the destructor, it must be called explicitly for now
		//so we can do things like copy shaders, make them assignable, etc.
	}

	unsigned Shader::Id() const
	{
		return id_;
	}

	void Shader::Destroy()
	{
		if (id_)
		{
			glDeleteShader(id_);
			id_ = 0;
		}
	}

	void Shader::Create(Target target, const char* label)
	{
		assert(!id_);


		if (id_)
		{
			opMo::DebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, id_, GL_DEBUG_SEVERITY_MEDIUM, -1, "Trying to compile a shader that has already been compiled");
			return;
		}

		target_ = target;
		id_ = glCreateShader(target_);
		opMo::ObjectLabel(GL_SHADER, id_, -1, label);
	}

	bool Shader::Recompile()
	{
		return Compile();
	}

	Shader::Target Shader::GetTarget() const
	{
		return target_;
	}

	void Shader::AddFile(const std::string& fn)
	{
		filenames_.push_back(fn);
	}

	bool Shader::Compile()
	{
		if (!id_)
		{
			opMo::DebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_MEDIUM, -1, "Trying to compile a shader before creating it");
			return false;
		}

		std::vector< std::string > vSource;

		bool success = true;
		unsigned failed = 0;

		for (unsigned i = 0; i < filenames_.size(); ++i)
		{

			vSource.push_back(std::string());
			success = LoadFile(filenames_[i], vSource.back());

			if (!success)
			{
				failed = i;
				break;
			}
		}

		if (!success)
		{
			std::cout << "Couldn't find file " << filenames_[failed] << std::endl;
		}

		const char** f = new const char*[vSource.size()];

		for (unsigned i = 0; i < vSource.size(); ++i)
		{
			f[i] = vSource[i].c_str();
		}

		glShaderSource(id_, vSource.size(), f, 0);

		glCompileShader(id_);

		CheckLog();

		delete[] f;

		return id_ != 0;
	}

	bool Shader::LoadFile(const std::string& filename, std::string& result)
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

	void Shader::CheckLog()
	{
		int logLength;

		glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0)
		{
			for (unsigned i = 0; i < filenames_.size(); ++i)
			{
				std::cout << filenames_[i] << "-------------------" << std::endl;
			}

			char *log = new char[logLength];
			glGetShaderInfoLog(id_, logLength, &logLength, log);
			std::cout << log;
			delete[] log;
		}

	}

	//GEOMETRY SHADER-------------

	//GeometryShader::GeometryShader() : Shader()
	//{ }
	//
	//GeometryShader::~GeometryShader() { }
	//
	//bool GeometryShader::Recompile()
	//{
	//	return Compile( target_, input_type_, output_type_, output_count_, filenames_ );
	//}
	//
	//bool GeometryShader::Compile( Target target, unsigned input_type, unsigned output_type, unsigned output_count, const std::vector< std::string >& fn, const char* label )
	//{
	//	bool r = Shader::Compile( target, fn, label );
	//	input_type_ = input_type;
	//	output_type_ = output_type;
	//	output_count_ = output_count;
	//	return r;
	//}
	//
	//bool GeometryShader::Compile( Target target, unsigned input_type, unsigned output_type, unsigned output_count, const std::string& fn, const char* label )
	//{
	//	bool r = Shader::Compile( target, fn, label );
	//	input_type_ = input_type;
	//	output_type_ = output_type;
	//	output_count_ = output_count;
	//	return r;
	//}
	//
	//unsigned GeometryShader::OutputCount() const
	//{
	//	return output_count_;
	//}
	//
	//unsigned GeometryShader::InputType() const
	//{
	//	return input_type_;
	//}
	//
	//unsigned GeometryShader::OutputType() const
	//{
	//	return output_type_;
	//}

	//PROGRAM -------------

	Program::Program(const char* name) : program_(0), transform_feedback_type_(GL_SEPARATE_ATTRIBS), m_name(name)
	{ }

	Program::Program(const Program& rhs)
	{
		Destroy();
		program_ = rhs.program_;
		std::copy(rhs.shaders_.begin(), rhs.shaders_.end(), shaders_.begin());
	}

	Program& Program::operator=(const Program& rhs)
	{
		Destroy();

		if (this != &rhs)
		{
			program_ = rhs.program_;
			std::copy(rhs.shaders_.begin(), rhs.shaders_.end(), shaders_.begin());
		}

		return *this;
	}

	Program::~Program()
	{
		//destroy should be called explicitly since we want to be assignable/copy constructable
	}

	void Program::Destroy()
	{
		for (unsigned i = 0; i < shaders_.size(); ++i)
		{
			if (shaders_[i]->Id())
			{
				shaders_[i]->Destroy();
			}
		}

		mUniforms_.swap(UniformMap());
		uniformVector_.swap(UniformVector());

		if (program_)
		{
			glDeleteProgram(program_);
			program_ = 0;
		}

		shaders_.clear();
	}

	void Program::Recompile()
	{
		std::for_each(shaders_.begin(), shaders_.end(), std::bind(&Shader::Recompile, std::placeholders::_1));

		Link();
	}

	bool Program::AddShader(std::shared_ptr<Shader> shader)
	{
		shaders_.push_back(shader);
		return true;
	}

	void Program::SetSamplerLoc(const char* uni_name, unsigned loc)
	{
		int mloc = glGetUniformLocationARB(program_, uni_name);
		glUniform1i(mloc, loc);
		samplerLocations_[uni_name] = loc;
	}

	int Program::GetUniformLoc(const char* uni_name) const
	{
		UniformMap::const_iterator itr = mUniforms_.find(uni_name);

		if (itr == mUniforms_.end())
			return -1;

		return itr->second->Location();
	}

	int Program::GetAttribLoc(const char* attrib_name) const
	{
		AttributeLocationMap::const_iterator itr = attributeLocations_.find(attrib_name);

		if (itr == attributeLocations_.end())
			return -1;

		return itr->second;
	}

	void Program::BindFragDataLoc(const char* data_name, unsigned loc)
	{
		glBindFragDataLocation(program_, loc, data_name);
		SetFragDataLoc(data_name, loc);
	}

	void Program::SetFragDataLoc(const char* data_name, unsigned loc)
	{
		fragmentDataLocation_[data_name] = loc;
	}

	bool Program::Link()
	{
		mUniforms_.swap(UniformMap());
		uniformVector_.swap(UniformVector());

		if (program_)
		{
			glDeleteProgram(program_);
			program_ = 0;
		}

		program_ = glCreateProgram();

		opMo::ObjectLabel(GL_PROGRAM, program_, -1, m_name.c_str());

		for (unsigned i = 0; i < shaders_.size(); ++i)
		{
			if (shaders_[i]->Id())
			{
				glAttachShader(program_, shaders_[i]->Id());

				//if( shaders_[i].GetTarget() == Shader::GEOM_SHADER )
				//{
				//	glProgramParameteriEXT( program_, GL_GEOMETRY_INPUT_TYPE_EXT, static_cast< const GeometryShader* >( &shaders_[i] )->InputType() );
				//	glProgramParameteriEXT( program_, GL_GEOMETRY_VERTICES_OUT_EXT, static_cast< const GeometryShader* >( &shaders_[i] )->OutputCount() );
				//	glProgramParameteriEXT( program_, GL_GEOMETRY_OUTPUT_TYPE_EXT, static_cast< const GeometryShader* >( &shaders_[i] )->OutputType() );
				//}
			}
		}

		if (!transform_feedbacks_.empty())
		{
			const char** xform_feed = new const char*[transform_feedbacks_.size()];

			for (unsigned i = 0; i < transform_feedbacks_.size(); ++i)
			{
				xform_feed[i] = transform_feedbacks_[i].c_str();
			}

			glTransformFeedbackVaryings(program_, transform_feedbacks_.size(), xform_feed, transform_feedback_type_);

			delete[] xform_feed;
		}

		glLinkProgram(program_);

		GLint charsWritten, infoLogLength;
		glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &infoLogLength);

		char * infoLog = new char[infoLogLength];
		glGetProgramInfoLog(program_, infoLogLength, &charsWritten, infoLog);
		printf(infoLog);
		delete[] infoLog;

		GLint success = GL_FALSE;
		glGetProgramiv(program_, GL_LINK_STATUS, &success);

		//if( success == GL_FALSE )
		//{
		//	glDeleteProgram( program_ );
		//	return false;
		//}

		int uniformMaxLength;
		glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &uniform_count_);
		glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

		GLint count = -1;
		GLenum type = 0;
		GLchar* uniformName = new GLchar[uniformMaxLength];

		for (GLint i = 0; i < uniform_count_; ++i)
		{
			GLsizei length;

			glGetActiveUniformName(program_, i, uniformMaxLength, &length, uniformName);

			int loc = glGetUniformLocation(program_, uniformName);

			uniformVector_.push_back(std::shared_ptr< UniformValue >(new UniformValue(uniformName, loc)));
			mUniforms_.insert(std::make_pair(uniformName, uniformVector_.back()));
		}

		delete[] uniformName;

		int attributeMaxLength;
		glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &attribute_count_);
		glGetProgramiv(program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength);

		GLchar* attributeName = new GLchar[attributeMaxLength];

		for (GLint i = 0; i < attribute_count_; ++i)
		{
			GLsizei length;
			GLint size;
			GLenum type;

			glGetActiveAttrib(program_, i, attributeMaxLength, &length, &size, &type, attributeName);

			int loc = glGetAttribLocation(program_, attributeName);

			attributeLocations_.insert(std::make_pair(attributeName, loc));
		}

		delete[] attributeName;

		Bind();

		for (SamplerLocationMap::iterator itr = samplerLocations_.begin(); itr != samplerLocations_.end(); ++itr)
		{
			int mloc = glGetUniformLocation(program_, itr->first.c_str());

			if (mloc != -1)
				glUniform1i(mloc, itr->second);
		}

		for (FragDataLocationMap::iterator itr = fragmentDataLocation_.begin(); itr != fragmentDataLocation_.end(); ++itr)
		{
			glBindFragDataLocation(program_, itr->second, itr->first.c_str());
		}

		Unbind();

		return true;

	}

	void Program::AddTransformFeedback(const char* strFeedback)
	{
		transform_feedbacks_.push_back(strFeedback);
	}

	void Program::ClearTransformFeedbacks()
	{
		transform_feedbacks_.swap(std::vector<std::string>());
	}

	void Program::SetTransformFeedbackType(unsigned uEnum)
	{
		transform_feedback_type_ = uEnum;
	}


	void Program::SetUniform(const std::string& sUniformName, const float* pValue, unsigned uSz)
	{
		UniformMap::iterator itr = mUniforms_.find(sUniformName);

		if (itr != mUniforms_.end())
		{
			itr->second->Update(pValue, uSz);
		}
	}

	void Program::SetUniformMatrix(const std::string& sUniformName, const float* pValue, unsigned uSz)
	{
		UniformMap::iterator itr = mUniforms_.find(sUniformName);

		if (itr != mUniforms_.end())
		{
			itr->second->SetIsMatrix(true);

			itr->second->Update(pValue, uSz);
		}
	}

	int Program::TotalUniforms() const
	{
		return uniformVector_.size();
	}

	const char* Program::UniformName(int index) const
	{
		return uniformVector_[index]->Name();
	}

	unsigned Program::Id() const
	{
		return program_;
	}

	void Program::Bind() const
	{
		glUseProgram(program_);

		for (UniformVector::const_iterator itr = uniformVector_.begin(); itr != uniformVector_.end(); ++itr)
			(*itr)->Apply();
	}

	void Program::Unbind() const
	{
		glUseProgram(0);
	}

}
