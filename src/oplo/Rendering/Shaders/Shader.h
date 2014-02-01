#ifndef OPLO_SHADER_H
#define OPLO_SHADER_H

#include "Rendering/oploGL.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace oplo
{

	class Shader
	{
	public:
		enum Target
		{
			UNKNOWN = 0,
			VTX_SHADER = GL_VERTEX_SHADER,
			FRAG_SHADER = GL_FRAGMENT_SHADER,
			GEOM_SHADER = GL_GEOMETRY_SHADER,
			TCS = GL_TESS_CONTROL_SHADER,
			TES = GL_TESS_EVALUATION_SHADER,
			COMPUTE_SHADER = GL_COMPUTE_SHADER
		};

		Shader(Target target, const char* label = "MysteryShader");

		~Shader();

		unsigned getId() const;

		Target getTarget() const;

		void create();

		void destroy();

		void addFile(const std::string& fn);

		void addInline(const std::string& inlinedCode);

		void clearInlined();

		void compile();

	protected:

		bool loadFile(const std::string& filename, std::string& source);

		void checkLog();

		Target m_target;

		unsigned m_id;

		std::string m_label;

		std::vector< std::string > m_inlined;

		std::vector< std::string > m_files;

	private:

		Shader(const Shader& rhs);
		Shader& operator=(const Shader& rhs);

	};
}

#endif
