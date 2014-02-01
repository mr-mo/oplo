#ifndef OPLO_UNIFORM_HANDLER_H
#define OPLO_UNIFORM_HANDLER_H

#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "Containers/HashMap/HashMap.h"
#include "Math/Vector.h"

namespace oplo
{
	class Program;

	class UniformHandler
	{
	public:

		UniformHandler();

		~UniformHandler();

		void recompile();

		void registerShader(std::shared_ptr< Program > program);

		void setUniform(const std::string& uni, float v);

		template<typename T, unsigned sz>
		void setUniform(const std::string& uni, Vector<T, sz> const& v)
		{
			setUniform(uni, reinterpret_cast<const char*>(v.begin()), sizeof(T)* sz);
		}

		void setUniform(const std::string& uni, const float* v, unsigned sz);

		void setUniform(const std::string& uni, const char* v, unsigned sz);

		void destroyUniformRegistry();

		void clearProgramRegistery();

	private:

		void addProgramUniformsToHash(Program& p);

		//memory owned by set, no need to keep two shared pointers
		typedef std::vector< Program* > ProgramVector;

		typedef DenseHashMap< std::string, ProgramVector*, StringParameters<QuadraticHashParameters<std::string> > > HashMap;

		typedef std::unordered_set< std::shared_ptr< Program >, std::function<std::size_t(std::shared_ptr<Program>)> > SetType;

		HashMap m_uniforms;

		SetType m_programs;
	};

}

#endif
