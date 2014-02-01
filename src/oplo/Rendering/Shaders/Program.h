#ifndef OPLO_PROGRAM_H
#define OPLO_PROGRAM_H

#include <Rendering\oploGL.h>
#include <string>
#include <memory>
#include <vector>

namespace oplo
{

	class Shader;
	class Program
	{
		Program(const Program& rhs);

		Program& operator=(const Program& rhs);

		struct Attribute
		{
			GLint m_location;
			GLint m_type;
			std::string m_name;
		};

	public:

		Program(const char* name);

		virtual ~Program();

		void destroy();

		void compile();

		void setUniform(const std::string& uniform, const char* value, unsigned sz);

		void setUniform(int idx, const char* value, unsigned sz);

		int getUniformLoc(const std::string& uni_name) const;

		int getInputLoc(const std::string&  attrib_name) const;

		int getOutputLoc(const std::string&  attrib_name) const;

		void addShader(std::shared_ptr<Shader> shader);

		void bind() const;

		void unbind() const;

		//void AddTransformFeedback(const char* strFeedback);

		//void ClearTransformFeedbacks();

		//void SetTransformFeedbackType(unsigned uEnum);

		unsigned getId() const;

		int getTotalUniforms() const;

		const std::string& getUniformName(int index) const;

	protected:

		virtual void compileEvent();

	private:

		bool link();

		void fillUniformData();

		void fillUniformBlocks();

		void fillIOData(GLenum target, GLint& tot, struct Attribute*& ar);

	private:

		typedef class UniformValue* UniformVector;

		typedef Attribute* IOArray;

		std::string m_name;

		GLint m_maxUniform;

		UniformVector m_uniforms;

		GLint m_maxInputs;

		IOArray m_inputs;

		GLint m_maxOutputs;

		IOArray m_outputs;



		std::vector< std::shared_ptr<Shader> > m_shaders;

		//	std::vector< std::string > transform_feedbacks_;

		//	unsigned transform_feedback_type_;

		//	AttributeLocationMap attributeLocations_;

		//	SamplerLocationMap samplerLocations_;

		//	FragDataLocationMap fragmentDataLocation_;

		unsigned m_id;
	};

}

#endif
