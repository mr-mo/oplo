#ifndef EPORT_SHADER_H
#define EPORT_SHADER_H

#include <vector>
#include <gl/glew.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include "Constants.h"


class Shader
{
public:
	enum Target
	{
		UNKNOWN = 0,
		VTX_SHADER = GL_VERTEX_SHADER,
		FRAG_SHADER = GL_FRAGMENT_SHADER,
		GEOM_SHADER = GL_GEOMETRY_SHADER
	};

	Shader();

	//This will automatically call create
	//Requires valid GL Context at the moment
	//TODO: Lazy eval
	Shader(Target target, const char* label);

	virtual ~Shader();

	unsigned Id() const;

	Target GetTarget() const;

	void Create(Target target, const char* label);

	void Destroy();

	bool Recompile();
	
	void AddFile(const std::string& fn);

	bool Compile();
	
protected:

	bool LoadFile( const std::string& filename, std::string& source );

	void CheckLog();

	Target target_;

	unsigned id_;

	std::vector< std::string > filenames_;

private:

	Shader(const Shader& rhs);
	Shader& operator=(const Shader& rhs);

};
//
//class GeometryShader : public Shader
//{
//public:
//	GeometryShader();
//	~GeometryShader();
//
//	bool Recompile();
//	bool Compile( Shader::Target target, const std::string& fn , const char* label);
//	bool Compile(Target target, unsigned input_type, unsigned output_type, unsigned output_count, const std::string& fn, const char* label);
//	bool Compile(Target target, unsigned input_type, unsigned output_type, unsigned output_count, const std::vector< std::string >& fn, const char* label);
//
//	unsigned OutputCount() const;
//	unsigned InputType() const;
//	unsigned OutputType() const;
//
//protected:
//
//	unsigned output_count_, input_type_, output_type_;
//
//private:
//
//	GeometryShader(const GeometryShader& rhs);
//	GeometryShader& operator=(const GeometryShader& rhs);
//
//};

class Program
{
	struct UniformValue
	{
		UniformValue( const std::string& name ) :
			  iUniformLoc_( -1 )
		    , name_( name )
			, bNeedsUpdate_( false )
			, bIsMatrix_( false )
		  { }

		UniformValue( const std::string& name, int loc ) :
			  iUniformLoc_( loc )
		    , name_( name )
			, bNeedsUpdate_( false )
			, bIsMatrix_( false )
		  { }

		UniformValue( const UniformValue& rhs ) :
			    iUniformLoc_( rhs.iUniformLoc_ )
			  , pCurrentValue_( rhs.pCurrentValue_ )
			  , bNeedsUpdate_( rhs.bNeedsUpdate_ )
			  , bIsMatrix_( rhs.bIsMatrix_ )
			  , name_( rhs.name_ )
		{}

		void ForceNeedsUpdate()
		{
			bNeedsUpdate_ = true;
		}

		bool Update( const float* pValue, unsigned uSz )
		{
			if( iUniformLoc_ == -1 )
				return false;

			if( uSz != pCurrentValue_.size() )
				pCurrentValue_.resize( uSz );

			for( unsigned i = 0; i < uSz; ++i )
			{
				if( pCurrentValue_[ i ] != pValue[ i ] )
					bNeedsUpdate_ = true;

				pCurrentValue_[ i ] = pValue[ i ];
			}

			return bNeedsUpdate_;
		}
		
		void Apply() const
		{
			if( !bNeedsUpdate_ )
				return;

			if( !bIsMatrix_ )
			{
				switch( pCurrentValue_.size() )
				{
				case 1:
					glUniform1f( iUniformLoc_, pCurrentValue_[ 0 ] );
					break;
				case 2:
					glUniform2f( iUniformLoc_, pCurrentValue_[ 0 ], pCurrentValue_[ 1 ] );
					break;
				case 3:
					glUniform3f( iUniformLoc_, pCurrentValue_[ 0 ], pCurrentValue_[ 1 ], pCurrentValue_[ 2 ] );
					break;
				case 4:
					glUniform4f( iUniformLoc_, pCurrentValue_[ 0 ], pCurrentValue_[ 1 ], pCurrentValue_[ 2 ], pCurrentValue_[ 3 ] );
					break;
				default:
					glUniform1fv( iUniformLoc_, GLsizei(pCurrentValue_.size()), &pCurrentValue_[ 0 ] );
					break;
				}
			}
			else
			{
				switch( pCurrentValue_.size() )
				{
				case 9:
					glUniformMatrix3fv( iUniformLoc_, 1, GL_FALSE, &pCurrentValue_[0] );
					break;
				case 16:
					glUniformMatrix4fv( iUniformLoc_, 1, GL_FALSE, &pCurrentValue_[0] );
					break;
				}
			}

			bNeedsUpdate_ = false;
		}

		void SetIsMatrix( bool bIsMatrix )
		{
			bIsMatrix_ = bIsMatrix;
		}

		void SetLocation( int iUniformLocation )
		{
			iUniformLoc_	 = iUniformLocation;
		}

		bool Valid() const
		{
			return iUniformLoc_ != -1;
		}

		const char* Name() const
		{
			return name_.c_str();
		}

		int Location() const
		{
			return iUniformLoc_;
		}

	private:

		std::string name_;

		int		iUniformLoc_;

		std::vector< float > pCurrentValue_;

		mutable bool	bNeedsUpdate_;

		bool	bIsMatrix_;
	};

public:

	enum ShaderType
	{
		VERTEX = 0,
		GEOMETRY,
		FRAGMENT,
		MAX_SHADER_TYPES
	};

	Program( const char* name );

	Program( const Program& rhs );

	Program& operator=( const Program& rhs );

	~Program();

	void Destroy();

	void Recompile();

	void SetSamplerLoc( const char* uni_name, unsigned loc );

	int GetAttributeLocation( const char* attrib_name ) const;

	void SetUniform( const std::string& sUniformName, const float* pValue, unsigned uSz );
	
	void SetUniformMatrix( const std::string& sUniformName, const float* pValue, unsigned uSz );

	void BindFragDataLoc( const char* data_name, unsigned loc );

	//called when Bind is called as well, will store location
	//and reuse on auto-recompile
	void SetFragDataLoc( const char* data_name, unsigned loc );

	int GetUniformLoc( const char* uni_name ) const;

	int GetAttribLoc( const char* attrib_name ) const;

	bool AddShader( std::shared_ptr<Shader> shader );

	bool Link();

	void Bind() const;

	void Unbind() const;

	void AddTransformFeedback( const char* strFeedback );

	void ClearTransformFeedbacks();

	void SetTransformFeedbackType( unsigned uEnum );

	unsigned Id() const;

	int TotalUniforms() const;

	const char* UniformName( int index ) const;

private:

	typedef std::unordered_map< std::string, std::shared_ptr< UniformValue > > UniformMap;
	
	typedef std::unordered_map< std::string, int > AttributeLocationMap;

	typedef std::unordered_map< std::string, int > SamplerLocationMap;

	typedef std::unordered_map< std::string, int > FragDataLocationMap;

	typedef std::vector< std::shared_ptr< UniformValue > > UniformVector;

	std::string m_name;

	int uniform_count_;

	int attribute_count_;

	std::vector< std::shared_ptr<Shader> > shaders_;

	std::vector< std::string > transform_feedbacks_;

	unsigned transform_feedback_type_;

	UniformMap mUniforms_;

	UniformVector uniformVector_;

	AttributeLocationMap attributeLocations_;

	SamplerLocationMap samplerLocations_;

	FragDataLocationMap fragmentDataLocation_;

	unsigned program_;
};

#endif
