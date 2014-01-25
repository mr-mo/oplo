#ifndef ERIK_PGRID_UNIFORM_HANDLER_H
#define ERIK_PGRID_UNIFORM_HANDLER_H

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "Constants.h"

class Program;

class UniformHandler
{
public:

	UniformHandler();

	~UniformHandler();

	void Recompile();

	void RegisterShader( std::shared_ptr< Program > pProgram );

	void SetUniform( const std::string& sUniformName, float fValue );

	void SetUniform( const std::string& sUniformName, float fValue0, float fValue1 );

	void SetUniform( const std::string& sUniformName, float fValue0, float fValue1, float fValue2 );

	void SetUniform( const std::string& sUniformName, float fValue0, float fValue1, float fValue2, float fValue3 );

	void SetUniform( const std::string& sUniformName, const float* pValue, unsigned uSize, bool bIsMatrix = false );

	void Destroy();

private:

	typedef std::vector< std::shared_ptr< Program > > ProgramVector;

	typedef std::unordered_map< std::string, ProgramVector > MapType;

	typedef std::unordered_set< std::shared_ptr< Program >, std::function<std::size_t(std::shared_ptr<Program>)> > SetType;

	MapType mUniforms_;

	SetType cRegisteredPrograms_;
};

#endif
