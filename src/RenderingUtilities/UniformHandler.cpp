#include "UniformHandler.h"
#include "Shader.h"
#include "Vector.h"

UniformHandler::UniformHandler() : cRegisteredPrograms_(3, [](std::shared_ptr<Program> p) { return reinterpret_cast<std::size_t>(p.get()); } )
{}

UniformHandler::~UniformHandler()
{}

void UniformHandler::Recompile()
{
	mUniforms_.clear();

	for( SetType::iterator itr = cRegisteredPrograms_.begin(); itr != cRegisteredPrograms_.end(); ++itr )
	{
		(*itr)->Recompile();

		for( int i = 0; i < (*itr)->TotalUniforms(); ++i )
			mUniforms_[ (*itr)->UniformName( i ) ].push_back( *itr );
	}
}

void UniformHandler::RegisterShader( std::shared_ptr< Program > pProgram )
{
	cRegisteredPrograms_.insert( pProgram );

	for( int i = 0; i < pProgram->TotalUniforms(); ++i )
		mUniforms_[ pProgram->UniformName( i ) ].push_back( pProgram );

	int bp = 3;
}

void UniformHandler::SetUniform( const std::string& sUniformName, float fValue )
{
	MapType::iterator itr = mUniforms_.find( sUniformName );

	if( itr != mUniforms_.end() )
	{
		for( ProgramVector::iterator vitr = itr->second.begin(); vitr != itr->second.end(); ++vitr )
		{
			(*vitr)->SetUniform( sUniformName, &fValue, 1 );
		}
	}
}

void UniformHandler::SetUniform( const std::string& sUniformName, float fValue0, float fValue1 )
{
	MapType::iterator itr = mUniforms_.find( sUniformName );

	Math::Vec2F v( fValue0, fValue1 );

	if( itr != mUniforms_.end() )
	{
		for( ProgramVector::iterator vitr = itr->second.begin(); vitr != itr->second.end(); ++vitr )
		{
			(*vitr)->SetUniform( sUniformName, &v[0], 2 );
		}
	}
}

void UniformHandler::SetUniform( const std::string& sUniformName, float fValue0, float fValue1, float fValue2 )
{
	MapType::iterator itr = mUniforms_.find( sUniformName );

	Math::Vec3F v( fValue0, fValue1, fValue2 );

	if( itr != mUniforms_.end() )
	{
		for( ProgramVector::iterator vitr = itr->second.begin(); vitr != itr->second.end(); ++vitr )
		{
			(*vitr)->SetUniform( sUniformName, &v[0], 3 );
		}
	}
}

void UniformHandler::SetUniform( const std::string& sUniformName, float fValue0, float fValue1, float fValue2, float fValue3 )
{
	MapType::iterator itr = mUniforms_.find( sUniformName );

	Math::Vec4F v( fValue0, fValue1, fValue2, fValue3 );

	if( itr != mUniforms_.end() )
	{
		for( ProgramVector::iterator vitr = itr->second.begin(); vitr != itr->second.end(); ++vitr )
		{
			(*vitr)->SetUniform( sUniformName, &v[0], 4 );
		}
	}
}

void UniformHandler::SetUniform( const std::string& sUniformName, const float* pValue, unsigned uSize, bool bIsMatrix )
{
	MapType::iterator itr = mUniforms_.find( sUniformName );

	if( itr != mUniforms_.end() )
	{
		for( ProgramVector::iterator vitr = itr->second.begin(); vitr != itr->second.end(); ++vitr )
		{
			if( bIsMatrix )
			{
				(*vitr)->SetUniformMatrix( sUniformName, pValue, uSize );
			}
			else
			{
				(*vitr)->SetUniform( sUniformName, pValue, uSize );
			}
		}
	}
}
