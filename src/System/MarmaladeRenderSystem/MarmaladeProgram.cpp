#	include "MarmaladeProgram.h"

#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

#	define VERTEX_ARRAY 0
#	define COLOR_ARRAY 1
#	define UV0_ARRAY 2
#	define UV1_ARRAY 3

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeProgram::MarmaladeProgram()
		: m_serviceProvider( nullptr )
		, m_program( 0 )
		, m_transformLocation( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeProgram::~MarmaladeProgram()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MarmaladeProgram::getServiceProvider()
	{ 
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeProgram::getName() const
	{ 
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeProgram::initialize( const ConstString & _name, const RenderShaderInterfacePtr & _vertexShader, const RenderShaderInterfacePtr & _fragmentShader )
	{
		m_name = _name;

		GLuint program = glCreateProgram();

		if( program == 0 )
		{
			return false;
		}

		if( _vertexShader != nullptr )
		{
			m_vertexShader = stdex::intrusive_static_cast<MarmaladeShaderPtr>(_vertexShader);

			m_vertexShader->attach( program );
		}

		if( _fragmentShader != nullptr )
		{
			m_fragmentShader = stdex::intrusive_static_cast<MarmaladeShaderPtr>(_fragmentShader);

			m_fragmentShader->attach( program );
		}
		
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, VERTEX_ARRAY, "inVert" ) );
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, COLOR_ARRAY, "inCol" ) );
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, UV0_ARRAY, "inUV0" ) );
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, UV1_ARRAY, "inUV1" ) );

		GLCALL( m_serviceProvider, glLinkProgram, ( program ) );

		GLint linked;
		GLCALL( m_serviceProvider, glGetProgramiv, ( program, GL_LINK_STATUS, &linked ) );

		if( linked == GL_FALSE )
		{
			GLchar errorLog[1024] = {0};
			GLCALL( m_serviceProvider, glGetProgramInfoLog, ( program, 1023, NULL, errorLog ) );

			LOGGER_ERROR( m_serviceProvider )("MarmaladeProgram::shaderProgram - shader linking error '%s'"
				, errorLog
				);

			return false;
		}
				
		GLCALLR( m_serviceProvider, m_transformLocation, glGetUniformLocation, (program, "mvpMat") );

		GLCALLR( m_serviceProvider, m_samplerLocation[0], glGetUniformLocation, ( program, "inSampler0" ) );
		GLCALLR( m_serviceProvider, m_samplerLocation[1], glGetUniformLocation, ( program, "inSampler1" ) );

		m_program = program;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::enable() const
	{
		GLCALL( m_serviceProvider, glUseProgram, (m_program) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::disable() const
	{
		GLCALL( m_serviceProvider, glUseProgram, (0) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const
	{
		m_mvpMat = _worldMatrix * _viewMatrix * _projectionMatrix;

		GLCALL( m_serviceProvider, glUniformMatrix4fv, (m_transformLocation, 1, GL_FALSE, m_mvpMat.buff()) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::bindTexture( uint32_t _index ) const
	{	
		int sampler = m_samplerLocation[_index];

		if( _index == 0 )
		{
			int samplerLocation;
			GLCALLR( m_serviceProvider, samplerLocation, glGetUniformLocation, (m_program, "inSampler0") );
			GLCALL( m_serviceProvider, glUniform1i, (samplerLocation, 0) );
		}
		else if( _index == 1 )
		{
			int samplerLocation;
			GLCALLR( m_serviceProvider, samplerLocation, glGetUniformLocation, (m_program, "inSampler1") );
			GLCALL( m_serviceProvider, glUniform1i, (samplerLocation, 1) );
		}
		
		//GLCALLR( m_serviceProvider, m_samplerLocation[1], glGetUniformLocation, (m_program, "inSampler1") );

		//GLCALL( m_serviceProvider, glUniform1i, (sampler, _index) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::finalize()
	{ 
		GLCALL( m_serviceProvider, glDeleteProgram, (m_program) );

		m_vertexShader = nullptr;
		m_fragmentShader = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
