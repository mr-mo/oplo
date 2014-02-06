#include "UniformHandler.h"
#include "Program.h"
#include "Math/Vector.h"

namespace oplo
{

	UniformHandler::UniformHandler() : m_programs(3, [](std::shared_ptr<Program> p) { return reinterpret_cast<std::size_t>(p.get()); })
	{}

	UniformHandler::~UniformHandler()
	{}

	void UniformHandler::recompile()
	{
		destroyUniformRegistry();

		for (auto s : m_programs)
		{
			s->compile();
			addProgramUniformsToHash(*s);
		}
	}

	void UniformHandler::registerShader(std::shared_ptr< Program > program)
	{
		if (m_programs.insert(program).second)
		{
			addProgramUniformsToHash(*program);
		}
	}

	void UniformHandler::setUniform(const std::string& uni, float v)
	{
		setUniform(uni, reinterpret_cast<const char*>(&v), sizeof(float));
	}

	void UniformHandler::setUniform(const std::string& uni, const float* v, unsigned sz)
	{
		setUniform(uni, reinterpret_cast<const char*>(v), sz * sizeof(float));
	}

	void UniformHandler::setUniform(const std::string& uni, const char* v, unsigned sz)
	{
		HashMap::Iterator itr = m_uniforms.find(uni);

		if (itr != m_uniforms.end())
		{
			for (auto s : *(*itr).second)
			{
				s->setUniform(uni, v, sz);
			}
		}
	}

	void UniformHandler::destroyUniformRegistry()
	{
		HashMap::Iterator i = m_uniforms.begin();

		while (i != m_uniforms.end())
		{
			delete i->second;
			i = m_uniforms.next(i);
		}

		m_uniforms.clear();
	}

	void UniformHandler::clearProgramRegistery()
	{
		m_programs.clear();
	}

	void UniformHandler::addProgramUniformsToHash(Program& p)
	{
		for (int i = 0; i < p.getTotalUniforms(); ++i)
		{
			std::string uniform = p.getUniformName(i);

			if (!m_uniforms.keyIsValid(uniform))
				continue;

			const HashMap::Iterator itr = m_uniforms.find(uniform);

			if (itr != m_uniforms.end())
			{
				itr->second->push_back(&p);
			}
			else
			{
				ProgramVector* vec = new ProgramVector;
				vec->push_back(&p);
				m_uniforms.insert(uniform, vec);
			}
		}
	}

}
