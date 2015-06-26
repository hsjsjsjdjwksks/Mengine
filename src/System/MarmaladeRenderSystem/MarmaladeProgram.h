#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "MarmaladeShader.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
	class MarmaladeProgram
		: public RenderProgramInterface
	{
	public:
		MarmaladeProgram();
		~MarmaladeProgram();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() override;

	public:
		const ConstString & getName() const override;

    public:
		bool initialize( const ConstString & _name, const RenderShaderInterfacePtr & _vertexShader, const RenderShaderInterfacePtr & _fragmentShader );
		void finalize();

	public:
		void enable() const;
		void disable() const;

		void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const;
		void bindTexture( uint32_t _textureInd ) const;

	protected:
		GLuint createShader_( GLenum type, const char * _source );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;

		GLuint m_program;

		MarmaladeShaderPtr m_vertexShader;
		MarmaladeShaderPtr m_fragmentShader;
		
		mutable mt::mat4f m_mvpMat;

		int m_transformLocation;
		int m_samplerLocation[2];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MarmaladeProgram> MarmaladeProgramPtr;
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

