#	include "RenderEngine.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ImageCodecInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/WatchdogInterface.h"

#	include "RenderSubTexture.h"

//#	include "Megatextures.h"

#	include "Core/RenderUtil.h"

#	include "Logger/Logger.h"

#	include "stdex/memorycopy.h"

#   include <memory.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderService, Menge::RenderServiceInterface, Menge::RenderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine()
		: m_serviceProvider(nullptr)
		, m_windowCreated(false)
		, m_vsync(false)
		, m_vbHandle2D(0)
		, m_ibHandle2D(0)
		, m_currentVBHandle(0)
		, m_currentIBHandle(0)
		, m_currentBaseVertexIndex(0)
		, m_currentTextureStages(0)
		, m_currentRenderCamera(nullptr)        
		, m_currentRenderViewport(nullptr)
		, m_currentVertexDeclaration(0)
		, m_maxVertexCount(0)
		, m_maxIndexCount(0)
		, m_depthBufferWriteEnable(false)
		, m_alphaBlendEnable(false)
		, m_alphaTestEnable(false)
		, m_debugMode(false)
		, m_currentStage(nullptr)
		, m_nullTexture(nullptr)
		, m_debugMaterial(nullptr)
		, m_renderVertexCount(0)
		, m_renderIndicesCount(0)
		, m_batchMode(ERBM_SMART)
		, m_currentMaterialId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::~RenderEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		RENDER_SYSTEM(m_serviceProvider)
			->setRenderListener( this );
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * RenderEngine::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::initialize( size_t _maxVertexCount, size_t _maxIndexCount )
	{
		m_maxVertexCount = _maxVertexCount;
		m_maxIndexCount = _maxIndexCount;        

		for( RenderIndices2D i = 0; i != MENGINE_RENDER_INDICES_QUAD; i += 6 )
		{   
			RenderIndices2D vertexOffset = i / 6 * 4;

			m_indicesQuad[i + 0] = vertexOffset + 0;
			m_indicesQuad[i + 1] = vertexOffset + 3;
			m_indicesQuad[i + 2] = vertexOffset + 1;
			m_indicesQuad[i + 3] = vertexOffset + 1;
			m_indicesQuad[i + 4] = vertexOffset + 3;
			m_indicesQuad[i + 5] = vertexOffset + 2;
		}

		for( RenderIndices2D i = 0; i != MENGINE_RENDER_INDICES_LINE; ++i )
		{
			m_indicesLine[i] = i;
		}

		this->setRenderSystemDefaults_();

		m_debugInfo.dips = 0;
		m_debugInfo.frameCount = 0;
		m_debugInfo.fillrate = 0.f;
		m_debugInfo.object = 0;
		m_debugInfo.triangle = 0;

		//m_megatextures = new Megatextures(2048.f, 2048.f, PF_A8R8G8B8);

		m_defaultRenderTarget = CONST_STRING_LOCAL(m_serviceProvider, "Window");

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::finalize()
	{   
		for( TArrayRenderObject::iterator
			it = m_renderObjects.begin(),
			it_end = m_renderObjects.end();
		it != it_end;
		++it )
		{
			RenderObject & ro = *it;

			ro.material = nullptr;
		}

		m_renderObjects.clear();
		m_renderPasses.clear();
		//m_textures.clear();

		m_nullTexture = nullptr;

		RENDER_SYSTEM(m_serviceProvider)->releaseVertexBuffer( m_vbHandle2D );
		RENDER_SYSTEM(m_serviceProvider)->releaseIndexBuffer( m_vbHandle2D );

		for( TVectorVertexBuffer::iterator
			it = m_vertexBuffer.begin(),
			it_end = m_vertexBuffer.end();
		it != it_end;
		++it )
		{
			VBHandle handle = *it;

			RENDER_SYSTEM(m_serviceProvider)
				->releaseVertexBuffer( handle );
		}

		m_vertexBuffer.clear();

		for( TVectorIndexBuffer::iterator
			it = m_indexBuffer.begin(),
			it_end = m_indexBuffer.end();
		it != it_end;
		++it )
		{
			IBHandle handle = *it;

			RENDER_SYSTEM(m_serviceProvider)
				->releaseIndexBuffer( handle );
		}

		m_indexBuffer.clear();



		//delete m_megatextures;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, int _bits, bool _fullscreen,
		WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;

		m_fullscreen = _fullscreen;

		LOGGER_INFO(m_serviceProvider)("RenderEngine::createRenderWindow:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, m_contentResolution.getWidth()
			, m_contentResolution.getHeight()
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.end.x
			, m_renderViewport.end.y
			, m_fullscreen
			);

		m_windowCreated = RENDER_SYSTEM(m_serviceProvider)
			->createRenderWindow( m_windowResolution, _bits, m_fullscreen, _winHandle, m_vsync, _FSAAType, _FSAAQuality );

		if( m_windowCreated == false )
		{
			return false;
		}

		if( this->createNullTexture_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::createRenderWindow invalid create null texture"
				);

			return false;
		}

		//m_debugInfo.megatextures = 0;

		if( this->recreate2DBuffers_() == false )
		{
			return false;
		}

		this->restoreRenderSystemStates_();
		this->prepare2D_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createNullTexture_()
	{
		size_t null_width = 4;
		size_t null_height = 4;
		size_t null_channels = 3;

		m_nullTexture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createTexture( null_width, null_height, null_channels, PF_UNKNOWN );

		if( m_nullTexture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createWhitePixelTexture_ invalid create null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = null_width;
		rect.bottom = null_height;

		int pitch = 0;
		unsigned char* textureData = m_nullTexture->lock( &pitch, rect, false );

		if( textureData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderTextureManager::createWhitePixelTexture_ invalid lock null texture %d:%d"
				, null_width
				, null_height
				);

			return false;
		}

		for( size_t it = 0; it != null_height; ++it )
		{
			unsigned char null_color = 0xFF;
			std::fill( textureData, textureData + pitch * it + null_width, null_color );
		}

		m_nullTexture->unlock();

		RENDERTEXTURE_SERVICE(m_serviceProvider)
			->cacheFileTexture( CONST_STRING_LOCAL(m_serviceProvider, "__null__"), m_nullTexture );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen )
	{
		m_windowResolution = _resolution;
		m_contentResolution = _contentResolution;
		m_renderViewport = _renderViewport;

		m_fullscreen = _fullscreen;

		LOGGER_INFO(m_serviceProvider)("RenderEngine::changeWindowMode:\nwindow resolution [%d, %d]\ncontent resolution [%d, %d]\nrender viewport [%f %f %f %f]\nfullscreen %d"
			, m_windowResolution.getWidth()
			, m_windowResolution.getHeight()
			, m_contentResolution.getWidth()
			, m_contentResolution.getHeight()
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.end.x
			, m_renderViewport.end.y
			, m_fullscreen
			);

		if( m_windowCreated == false )
		{
			return;
		}

		RENDER_SYSTEM(m_serviceProvider)
			->changeWindowMode( m_windowResolution, m_fullscreen );

		//this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::screenshot( const RenderTextureInterfacePtr & _texture, const mt::vec4f & _rect )
	{
		const RenderImageInterfacePtr & image = _texture->getImage();

		RENDER_SYSTEM(m_serviceProvider)
			->screenshot( image, _rect.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onRenderSystemDeviceLost()
	{
		LOGGER_WARNING(m_serviceProvider)("RenderEngine::onRenderSystemDeviceLost"
			);

		m_currentVBHandle = 0;
		m_currentIBHandle = 0;

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::onRenderSystemDeviceRestored()
	{
		LOGGER_WARNING(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored"
			);

		if( this->recreate2DBuffers_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::onRenderSystemDeviceRestored invalid recreate buffers"
				);

			return false;
		}

		this->restoreRenderSystemStates_();
		this->prepare2D_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::onWindowClose()
	{
		if( m_windowCreated )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->onWindowClose();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene()
	{		
		m_renderPasses.clear();
		m_renderObjects.clear();
		m_debugRenderVertex2D.clear();

		m_debugInfo.fillrate = 0.f;
		m_debugInfo.object = 0;
		m_debugInfo.triangle = 0;

		m_currentRenderCamera = nullptr;
		m_currentRenderViewport = nullptr;
		
		m_currentMaterialId = 0;
		m_currentStage = nullptr;

		m_renderVertexCount = 0;
		m_renderIndicesCount = 0;

		m_currentRenderTarget = m_defaultRenderTarget;

		if( RENDER_SYSTEM(m_serviceProvider)->beginScene() == false )
		{
			return false;
		}
		//m_interface->clearFrameBuffer( FBT_COLOR );
		//m_interface->setRenderViewport( m_currentRenderViewport );

		//RENDER_SYSTEM(m_serviceProvider)->setShader( m_shader );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::endScene()
	{
		this->flushRender_();

		RENDER_SYSTEM(m_serviceProvider)
			->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::swapBuffers()
	{
		RENDER_SYSTEM(m_serviceProvider)
			->swapBuffers();

		m_debugInfo.frameCount += 1;		
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTarget( const ConstString& _target, bool _clear )
	{
		(void)_clear;

		m_currentRenderTarget = _target;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& RenderEngine::getRenderTarget() const
	{
		return m_currentRenderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::isWindowCreated() const
	{
		return m_windowCreated;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateStage_( const RenderStage * _stage )
	{
		if( m_currentStage == _stage )
		{
			return;
		}
		
		m_currentStage = _stage;

		for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			RenderTextureStage & current_texture_stage = m_currentTextureStage[stageId];
			const RenderTextureStage & texture_stage = m_currentStage->textureStage[stageId];

			if( current_texture_stage.addressU != texture_stage.addressU
				|| current_texture_stage.addressV != texture_stage.addressV )
			{
				current_texture_stage.addressU = texture_stage.addressU;
				current_texture_stage.addressV = texture_stage.addressV;

				RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( stageId
					, current_texture_stage.addressU
					, current_texture_stage.addressV 
					);
			}

			if( current_texture_stage.colorOp != texture_stage.colorOp
				|| current_texture_stage.colorArg1 != texture_stage.colorArg1
				|| current_texture_stage.colorArg2 != texture_stage.colorArg2 )
			{
				current_texture_stage.colorOp = texture_stage.colorOp;
				current_texture_stage.colorArg1 = texture_stage.colorArg1;
				current_texture_stage.colorArg2 = texture_stage.colorArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( stageId
					, current_texture_stage.colorOp
					, current_texture_stage.colorArg1
					, current_texture_stage.colorArg2 
					);
			}

			if( current_texture_stage.alphaOp != texture_stage.alphaOp
				|| current_texture_stage.alphaArg1 != texture_stage.alphaArg1
				|| current_texture_stage.alphaArg2 != texture_stage.alphaArg2 )
			{
				current_texture_stage.alphaOp = texture_stage.alphaOp;
				current_texture_stage.alphaArg1 = texture_stage.alphaArg1;
				current_texture_stage.alphaArg2 = texture_stage.alphaArg2;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( stageId
					, current_texture_stage.alphaOp
					, current_texture_stage.alphaArg1
					, current_texture_stage.alphaArg2
					);
			}

			if( current_texture_stage.texCoordIndex != texture_stage.texCoordIndex )
			{
				current_texture_stage.texCoordIndex = texture_stage.texCoordIndex;

				RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( stageId
					, current_texture_stage.texCoordIndex
					);
			}
		}

		if( m_depthBufferWriteEnable != m_currentStage->depthBufferWriteEnable )
		{
			m_depthBufferWriteEnable = m_currentStage->depthBufferWriteEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		}

		if( m_alphaTestEnable != m_currentStage->alphaTestEnable )
		{
			m_alphaTestEnable = m_currentStage->alphaTestEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setAlphaTestEnable( m_alphaTestEnable );
		}

		if( m_alphaBlendEnable != m_currentStage->alphaBlendEnable )
		{
			m_alphaBlendEnable = m_currentStage->alphaBlendEnable;

			RENDER_SYSTEM(m_serviceProvider)
				->setAlphaBlendEnable( m_alphaBlendEnable );
		}

		if( m_currentBlendSrc != m_currentStage->blendSrc )
		{
			m_currentBlendSrc = m_currentStage->blendSrc;

			RENDER_SYSTEM(m_serviceProvider)
				->setSrcBlendFactor( m_currentBlendSrc );
		}

		if( m_currentBlendDst != m_currentStage->blendDst )
		{
			m_currentBlendDst = m_currentStage->blendDst;

			RENDER_SYSTEM(m_serviceProvider)
				->setDstBlendFactor( m_currentBlendDst );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateTexture_( size_t _stageId, const RenderTextureInterfacePtr & _texture )
	{
		size_t texture_id = _texture->getId();
		size_t current_texture_id = m_currentTexturesID[_stageId];

		if( texture_id != current_texture_id || current_texture_id != 0 )
		{
			m_currentTexturesID[_stageId] = texture_id;

			const RenderImageInterfacePtr & image = _texture->getImage();

			RENDER_SYSTEM(m_serviceProvider)
				->setTexture( _stageId, image );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::updateMaterial_( const RenderMaterialPtr & _material )
	{		
		size_t materialId = _material->getId();

		if( m_currentMaterialId == materialId )
		{
			return;
		}

		m_currentMaterialId = materialId;

		size_t textureCount = _material->getTextureCount();

		if( m_currentTextureStages > textureCount )
		{
			for( size_t stageId = textureCount; stageId != m_currentTextureStages; ++stageId )
			{
				this->disableTextureStage_( stageId );
			}
		}

		m_currentTextureStages = textureCount;

		for( size_t stageId = 0; stageId != m_currentTextureStages; ++stageId )
		{
			const RenderTextureInterfacePtr & texture = _material->getTexture( stageId );

			if( texture == nullptr )
			{
				this->updateTexture_( stageId, m_nullTexture );
			}
			else
			{
				this->updateTexture_( stageId, texture );
			}
		}

		const RenderStage * stage = _material->getStage();

		this->updateStage_( stage );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObject_( RenderObject* _renderObject )
	{
		if( _renderObject->dipVerticesNum == 0 )
		{
			return;
		}

		const RenderMaterialPtr & material = _renderObject->material;

		this->updateMaterial_( material );		

		if( m_currentIBHandle != _renderObject->ibHandle || 
			m_currentBaseVertexIndex != _renderObject->baseVertexIndex )
		{
			m_currentIBHandle = _renderObject->ibHandle;
			m_currentBaseVertexIndex = _renderObject->baseVertexIndex;

			RENDER_SYSTEM(m_serviceProvider)
				->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		}

		if( m_currentVBHandle != _renderObject->vbHandle )
		{
			m_currentVBHandle = _renderObject->vbHandle;

			RENDER_SYSTEM(m_serviceProvider)
				->setVertexBuffer( m_currentVBHandle );
		}

		EPrimitiveType primitiveType = material->getPrimitiveType();

		RENDER_SYSTEM(m_serviceProvider)->drawIndexedPrimitive( 
			primitiveType,
			_renderObject->baseVertexIndex, 
			_renderObject->minIndex,
			_renderObject->dipVerticesNum, 
			_renderObject->startIndex, 
			_renderObject->dipIndiciesNum 
			);

		//if( _renderObject->material->getTextureCount() > 0 )
		//{
		//	printf("DIP %d %s\n"
		//		, _renderObject->material->getId()
		//		, _renderObject->material->getTexture(0)->getFileName().c_str()
		//		);
		//}
		//else
		//{
		//	printf("DIP debug!\n");
		//}

		_renderObject->material = nullptr;

		++m_debugInfo.dips;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::disableTextureStage_( size_t _stage )
	{
		RenderTextureStage & stage = m_currentTextureStage[_stage];

		stage = RenderTextureStage();

		m_currentTexturesID[_stage] = 0;

		RENDER_SYSTEM(m_serviceProvider)
			->setTexture( _stage, nullptr );

		RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( _stage
			, stage.addressU
			, stage.addressV 
			);

		RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( _stage
			, stage.colorOp
			, stage.colorArg1
			, stage.colorArg2
			);

		RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( _stage
			, stage.alphaOp
			, stage.alphaArg1
			, stage.alphaArg2
			);

		RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( _stage
			, 0 
			);

		RENDER_SYSTEM(m_serviceProvider)
			->setTextureMatrix( _stage, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::restoreRenderSystemStates_()
	{
		mt::mat4f viewTransform;
		mt::mat4f projTransform;
		mt::mat4f worldTransform;

		mt::ident_m4( viewTransform );
		mt::ident_m4( projTransform );
		mt::ident_m4( worldTransform );

		RENDER_SYSTEM(m_serviceProvider)->setVertexBuffer( m_currentVBHandle );
		RENDER_SYSTEM(m_serviceProvider)->setIndexBuffer( m_currentIBHandle, m_currentBaseVertexIndex );
		RENDER_SYSTEM(m_serviceProvider)->setVertexDeclaration( sizeof(RenderVertex2D), Vertex2D_declaration );
		RENDER_SYSTEM(m_serviceProvider)->setProjectionMatrix( projTransform );
		RENDER_SYSTEM(m_serviceProvider)->setModelViewMatrix( viewTransform );
		RENDER_SYSTEM(m_serviceProvider)->setWorldMatrix( worldTransform );
		RENDER_SYSTEM(m_serviceProvider)->setCullMode( CM_CULL_NONE );
		RENDER_SYSTEM(m_serviceProvider)->setFillMode( FM_SOLID );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferTestEnable( false );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferWriteEnable( m_depthBufferWriteEnable );
		RENDER_SYSTEM(m_serviceProvider)->setDepthBufferCmpFunc( CMPF_LESS_EQUAL );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaTestEnable( m_alphaTestEnable );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaBlendEnable( m_alphaBlendEnable );
		RENDER_SYSTEM(m_serviceProvider)->setAlphaCmpFunc( CMPF_GREATER_EQUAL, 0x01 );
		RENDER_SYSTEM(m_serviceProvider)->setLightingEnable( false );

		LOGGER_INFO(m_serviceProvider)("RenderEngine::restoreRenderSystemStates_ texture stages %d"
			, MENGE_MAX_TEXTURE_STAGES
			);

		for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			RenderTextureStage & stage = m_currentTextureStage[i];

			RENDER_SYSTEM(m_serviceProvider)->setTextureAddressing( i, stage.addressU, stage.addressV );

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageColorOp( i
				, stage.colorOp
				, stage.colorArg1
				, stage.colorArg2 
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageAlphaOp( i
				, stage.alphaOp
				, stage.alphaArg1
				, stage.alphaArg2 
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageTexCoordIndex( i
				, stage.texCoordIndex
				);

			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MIPMAP, TF_LINEAR );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MAGNIFICATION, TF_LINEAR );
			RENDER_SYSTEM(m_serviceProvider)->setTextureStageFilter( i, TFT_MINIFICATION, TF_LINEAR );

			// skip texture matrix
			RENDER_SYSTEM(m_serviceProvider)->setTextureMatrix( i, NULL );
		}

		RENDER_SYSTEM(m_serviceProvider)->setSrcBlendFactor( m_currentBlendSrc );
		RENDER_SYSTEM(m_serviceProvider)->setDstBlendFactor( m_currentBlendDst );

		//InputStreamInterfacePtr shaderStream = FILE_SERVICE(m_serviceProvider)
		//    ->openInputFile( ConstString::none(), Helper::stringizeString( m_serviceProvider, "shader.bin" ) );

		//size_t shaderSize = shaderStream->size();

		//char * shaderBuff = new char [shaderSize];

		//shaderStream->read( shaderBuff, shaderSize );

		//RenderShaderInterface * shader = RENDER_SYSTEM(m_serviceProvider)
		//    ->createShader( shaderBuff, shaderSize );

		//m_shader = shader;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionOrthogonal( mt::mat4f& _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionOrthogonal( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionPerspective( _projectionMatrix, _fov, _aspect, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float zn, float zf )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeProjectionFrustum( _projectionMatrix, _viewport, zn, zf );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixFromViewport( mt::mat4f& _viewMatrix, const Viewport & _viewport )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeViewMatrixFromViewport( _viewMatrix, _viewport );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->makeViewMatrixLookAt( _viewMatrix, _eye, _dir, _up, _sign );
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderDebugInfo& RenderEngine::getDebugInfo() const
	{
		return m_debugInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::resetFrameCount()
	{
		m_debugInfo.frameCount = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPasses_()
	{
		if( m_renderPasses.empty() == true )	// nothing to render
		{
			return;
		}

		for( TArrayRenderPass::iterator
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass & renderPass = *it;

			this->renderPass_( renderPass );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderPass_( RenderPass & _renderPass )
	{
		const RenderCameraInterface * camera = _renderPass.camera;

		const ConstString& renderTarget = camera->getRenderTarget();

		if( renderTarget != m_currentRenderTarget && renderTarget.empty() == false )
		{
			m_currentRenderTarget = renderTarget;

			RENDER_SYSTEM(m_serviceProvider)
				->setRenderTarget( nullptr, true );
		}

		float renderWidth = m_renderViewport.getWidth();
		float renderHeight = m_renderViewport.getHeight();

		//float viewportWidth = viewport.getWidth();
		//float viewportHeight = viewport.getHeight();

		size_t contentWidth = m_contentResolution.getWidth();
		size_t contentHeight = m_contentResolution.getHeight();

		//float scale_width = renderWidth / viewportWidth;
		//float scale_height = renderHeight / viewportHeight;

		float scale_width = renderWidth / float(contentWidth);
		float scale_height = renderHeight / float(contentHeight);

		const Viewport & viewport = _renderPass.viewport->getViewport();

		Viewport renderViewport;
		renderViewport.begin.x = viewport.begin.x * scale_width;
		renderViewport.begin.y = viewport.begin.y * scale_height;
		renderViewport.end.x = viewport.end.x * scale_width;
		renderViewport.end.y = viewport.end.y * scale_height;

		renderViewport.begin += m_renderViewport.begin;
		renderViewport.end += m_renderViewport.begin;

		//renderViewport.begin = m_renderOffset + viewport.begin * m_renderScale;
		//renderViewport.end = m_renderOffset + viewport.end * m_renderScale;

		float vp_x = ::floorf( renderViewport.begin.x + 0.5f );
		float vp_y = ::floorf( renderViewport.begin.y + 0.5f );

		float width = renderViewport.getWidth();
		float height = renderViewport.getHeight();

		float vp_width = ::floorf( width + 0.5f );
		float vp_height = ::floorf( height + 0.5f );

		mt::vec2f windowSize;
		m_windowResolution.calcSize( windowSize );
		
		if( vp_x >= windowSize.x || 
			vp_y >= windowSize.y ||
			vp_x + vp_width <= 0.f || 
			vp_y + vp_height <= 0.f )
		{
			renderViewport.begin.x = 0.f;
			renderViewport.begin.y = 0.f;
			renderViewport.end.x = 0.f;
			renderViewport.end.y = 0.f;
		}
		else
		{
			if( vp_x < 0.f )
			{
				renderViewport.begin.x = 0.f;
			}
			
			if( vp_x + vp_width > windowSize.x )
			{
				renderViewport.end.x = windowSize.x;
			}

			if( vp_y < 0.f )
			{
				renderViewport.begin.y = 0.f;
			}
			
			if( vp_y + vp_height > windowSize.y )
			{
				renderViewport.end.y = windowSize.y;
			}
		}

		RENDER_SYSTEM(m_serviceProvider)
			->setViewport( renderViewport );

		const mt::mat4f & worldMatrix = camera->getCameraWorldMatrix();

		RENDER_SYSTEM(m_serviceProvider)
			->setWorldMatrix( worldMatrix );

		const mt::mat4f & viewMatrix = camera->getCameraViewMatrix();

		RENDER_SYSTEM(m_serviceProvider)
			->setModelViewMatrix( viewMatrix );

		const mt::mat4f & projectionMatrix = camera->getCameraProjectionMatrix();

		RENDER_SYSTEM(m_serviceProvider)
			->setProjectionMatrix( projectionMatrix );

		this->renderObjects_( _renderPass );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderObjects_( RenderPass & _renderPass )
	{
		TArrayRenderObject::iterator it_begin = m_renderObjects.advance( _renderPass.beginRenderObject );
		TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass.beginRenderObject + _renderPass.countRenderObject );

		for( ;it_begin != it_end; ++it_begin )
		{
			RenderObject * renderObject = it_begin;

			this->renderObject_( renderObject );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	inline static void s_setupBoxRenderObject( mt::box2f & _bx, const RenderObject * _ro )
	{
		const RenderVertex2D & v0 = _ro->vertexData[0];
		mt::reset( _bx, v0.pos.x, v0.pos.y );

		for( size_t i = 1; i != _ro->verticesNum; ++i )
		{
			const RenderVertex2D & v = _ro->vertexData[i];

			mt::add_internal_point( _bx, v.pos.x, v.pos.y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderPass_()
	{
		RenderPass & pass = m_renderPasses.emplace();
		pass.beginRenderObject = m_renderObjects.size();
		pass.countRenderObject = 0;
		pass.viewport = m_currentRenderViewport;
		pass.camera = m_currentRenderCamera;

		const Viewport & rp = pass.camera->getCameraRenderport();

		const mt::mat4f & vm = pass.camera->getCameraViewMatrix();

		mt::mat4f inv_vm;
		mt::inv_m4( inv_vm, vm );

		Viewport rp_vm;
		mt::mul_v2_m4( rp_vm.begin, rp.begin, inv_vm );
		mt::mul_v2_m4( rp_vm.end, rp.end, inv_vm );

		mt::box2f bb_vp;
		rp_vm.toBBox(bb_vp);

		pass.bb = bb_vp;
		pass.orthogonalProjection = pass.camera->isOrthogonalProjection();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderObject( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material        
		, const RenderVertex2D * _vertices, size_t _verticesNum
		, const RenderIndices2D * _indices, size_t _indicesNum
		, const mt::box2f * _bb )
	{
		if( _viewport == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D viewport == NULL"
				);

			return;
		}

		if( _camera == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D camera == NULL"
				);

			return;
		}

		if( _material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D _material == NULL"
				);

			return;
		}

		if( m_renderObjects.full() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D max render objects %d"
				, MENGINE_RENDER_OBJECTS_MAX
				);

			return;
		}

		if( _vertices == nullptr )
		{
			return;
		}

		if( _indices == nullptr )
		{
			return;
		}

		if( m_currentRenderCamera != _camera || m_currentRenderViewport != _viewport )
		{
			if( m_renderPasses.full() == true )
			{
				LOGGER_ERROR(m_serviceProvider)("RenderEngine::renderObject2D max render passes %d"
					, MENGINE_RENDER_PASS_MAX
					);

				return;
			}

			m_currentRenderCamera = _camera;
			m_currentRenderViewport = _viewport;

			this->addRenderPass_();
		}

		RenderPass & rp = m_renderPasses.back();

		mt::box2f bb;

		if( rp.orthogonalProjection == true )
		{
			if( _bb != nullptr )
			{
				bb = *_bb;
			}
			else
			{
				mt::box2f bb_vertices;
				Helper::makeRenderBoundingBox( bb_vertices, _vertices, _verticesNum );

				const mt::mat4f & wm = rp.camera->getCameraWorldMatrix();

				mt::mul_v2_m4( bb.minimum, bb_vertices.minimum, wm );
				mt::mul_v2_m4( bb.maximum, bb_vertices.maximum, wm );
			}

			if( mt::is_intersect( rp.bb, bb ) == false )
			{
				return;
			}
		}

		++rp.countRenderObject;

		RenderObject & ro = m_renderObjects.emplace();

		ro.material = _material;

		ro.ibHandle = m_ibHandle2D;
		ro.vbHandle = m_vbHandle2D;

		ro.vertexData = _vertices;
		ro.verticesNum = _verticesNum;

		ro.indicesData = _indices;
		ro.indicesNum = _indicesNum;

		ro.minIndex = 0;
		ro.startIndex = 0;
		ro.baseVertexIndex = 0;

		ro.dipVerticesNum = 0;
		ro.dipIndiciesNum = 0;

		ro.bb = bb;

		if( m_debugMode == true )
		{
			EPrimitiveType primitiveType = ro.material->getPrimitiveType();

			switch( primitiveType )
			{
			case PT_TRIANGLELIST:
				{
					this->calcQuadSquare_( _vertices, _verticesNum );
				}break;
			}
		}

		m_renderVertexCount += _verticesNum;
		m_renderIndicesCount += _indicesNum;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderQuad( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material
		, const RenderVertex2D * _vertices, size_t _verticesNum
		, const mt::box2f * _bb )
	{
		size_t indicesNum = (_verticesNum / 4) * 6;

		if( indicesNum >= MENGINE_RENDER_INDICES_QUAD )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderQuad count %d > max count %d"
				, indicesNum
				, MENGINE_RENDER_INDICES_QUAD
				);

			return;
		}

		this->addRenderObject( _viewport, _camera, _material, _vertices, _verticesNum, m_indicesQuad, indicesNum, _bb );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::addRenderLine( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material
		, const RenderVertex2D * _vertices, size_t _verticesNum
		, const mt::box2f * _bb )
	{
		size_t indicesNum = _verticesNum;

		if( indicesNum >= MENGINE_RENDER_INDICES_LINE )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::addRenderLine count %d > max count %d"
				, indicesNum
				, MENGINE_RENDER_INDICES_LINE
				);

			return;
		}

		this->addRenderObject( _viewport, _camera, _material, _vertices, _verticesNum, m_indicesLine, indicesNum, _bb );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial )
	{
		m_debugMaterial = _debugMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderMaterialInterfacePtr & RenderEngine::getDebugMaterial() const
	{
		return m_debugMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertex2D * RenderEngine::getDebugRenderVertex2D( size_t _count )
	{
		size_t renderVertex2DSize = m_debugRenderVertex2D.size();
		size_t renderVertex2DCapacity = m_debugRenderVertex2D.capacity();

		if( renderVertex2DSize + _count > renderVertex2DCapacity )
		{
			return nullptr;
		}

		RenderVertex2D * vertices = m_debugRenderVertex2D.emplace_count( _count );

		return vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setBatchMode( ERenderBatchMode _mode )
	{
		m_batchMode = _mode;
	}
	ERenderBatchMode RenderEngine::getBatchMode() const
	{
		return m_batchMode;
	}
	//////////////////////////////////////////////////////////////////////////
	VBHandle RenderEngine::createVertexBuffer( const RenderVertex2D * _buffer, size_t _count )
	{
		VBHandle vbHandle = RENDER_SYSTEM(m_serviceProvider)
			->createVertexBuffer( _count, sizeof(RenderVertex2D), true );

		if( vbHandle == 0 )
		{
			return 0;
		}

		m_vertexBuffer.push_back(vbHandle);

		this->updateVertexBuffer( vbHandle, _buffer, _count );

		return vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle RenderEngine::createIndicesBuffer( const RenderIndices2D * _buffer, size_t _count )
	{
		IBHandle ibHandle = RENDER_SYSTEM(m_serviceProvider)
			->createIndexBuffer( _count, false );

		if( ibHandle == 0 )
		{
			return 0;
		}

		m_indexBuffer.push_back( ibHandle );

		this->updateIndicesBuffer( ibHandle, _buffer, _count );

		return ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseVertexBuffer( VBHandle _handle )
	{
		TVectorVertexBuffer::iterator it_found = std::find( m_vertexBuffer.begin(), m_vertexBuffer.end(), _handle );

		if( it_found == m_vertexBuffer.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to release vertex buffer" 
				);

			return;
		}

		m_vertexBuffer.erase( it_found );

		RENDER_SYSTEM(m_serviceProvider)
			->releaseVertexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseIndicesBuffer( IBHandle _handle )
	{
		TVectorIndexBuffer::iterator it_found = std::find( m_indexBuffer.begin(), m_indexBuffer.end(), _handle );

		if( it_found == m_indexBuffer.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to release index buffer" 
				);

			return;
		}

		m_indexBuffer.erase( it_found );

		RENDER_SYSTEM(m_serviceProvider)
			->releaseIndexBuffer( _handle );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateVertexBuffer( VBHandle _handle, const RenderVertex2D * _buffer, size_t _count  )
	{
		void * vbuffer = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
			_handle
			, 0
			, _count * sizeof(RenderVertex2D)
			, 0 
			);

		if( vbuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to lock vertex buffer"
				);

			return false;
		}

		stdex::memorycopy( vbuffer, _buffer, _count * sizeof(RenderVertex2D) );
		//std::copy( _buffer + 0, _buffer + _count, static_cast<RenderVertex2D*>(vbuffer) );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( _handle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine: failed to unlock vertex buffer" 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::updateIndicesBuffer( IBHandle _handle, const RenderIndices2D * _buffer, size_t _count )
	{
		void * ibuffer = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( _handle, 0, _count, 0 );

		if( ibuffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to lock vertex buffer"
				);

			return false;
		}

		stdex::memorycopy( ibuffer, _buffer, _count * sizeof(RenderIndices2D) );
		//std::copy( _buffer + 0, _buffer + _count, static_cast<uint16_t*>(ibuffer) );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( _handle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::updateIndicesBuffer: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::makeBatches_()
	{		
		if( m_renderVertexCount >= m_maxVertexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: vertex buffer overflow"
				);

			return false;
		}

		if( m_renderIndicesCount >= m_maxIndexCount )
		{
			LOGGER_WARNING(m_serviceProvider)("RenderEngine::makeBatches_: indices buffer overflow"
				);

			return false;
		}

		uint32_t vbLockFlags = LOCK_DISCARD;

		void * vbData = RENDER_SYSTEM(m_serviceProvider)->lockVertexBuffer( 
			m_vbHandle2D
			, 0
			, m_renderVertexCount * sizeof(RenderVertex2D)
			, vbLockFlags 
			);

		if( vbData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock vertex buffer"
				);

			return false;
		}

		RenderVertex2D * vertexBuffer = static_cast<RenderVertex2D *>(vbData);

		uint32_t ibLockFlags = LOCK_DISCARD;
		//uint32 ibLockFlags = 0;

		void * ibData = RENDER_SYSTEM(m_serviceProvider)->lockIndexBuffer( 
			m_ibHandle2D
			, 0
			, m_renderIndicesCount * sizeof(RenderIndices2D)
			, ibLockFlags 
			);

		if( ibData == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to lock indices buffer"
				);

			return false;
		}

		RenderIndices2D * indicesBuffer = static_cast<RenderIndices2D *>(ibData);

		this->insertRenderPasses_( vertexBuffer, indicesBuffer, m_renderVertexCount, m_renderIndicesCount );

		if( RENDER_SYSTEM(m_serviceProvider)->unlockIndexBuffer( m_ibHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to unlock indices buffer"
				);

			return false;
		}

		if( RENDER_SYSTEM(m_serviceProvider)->unlockVertexBuffer( m_vbHandle2D ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::makeBatches_: failed to unlock vertex buffer"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderPasses_( RenderVertex2D * _vertexBuffer, RenderIndices2D * _indicesBuffer, size_t _vbSize, size_t _ibSize )
	{
		size_t vbPos = 0;
		size_t ibPos = 0;

		for( TArrayRenderPass::iterator 
			it = m_renderPasses.begin(), 
			it_end = m_renderPasses.end();
		it != it_end;
		++it )
		{
			RenderPass * pass = &(*it);

			this->insertRenderObjects_( pass, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::batchRenderObjectNormal_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices2D * _indicesBuffer, size_t _vbSize, size_t _ibSize, size_t & _vbPos, size_t & _ibPos )
	{
		size_t vbPos = _vbPos;
		size_t ibPos = _ibPos;

		TArrayRenderObject::iterator it_batch_begin = _begin;
		++it_batch_begin;

		for( ; it_batch_begin != _end; ++it_batch_begin )
		{
			RenderObject * ro_bath_begin = it_batch_begin;

			if( ro_bath_begin->verticesNum == 0 )
			{
				continue;
			}

			if( _ro->material != ro_bath_begin->material )
			{
				break;
			}

			this->insertRenderObject_( ro_bath_begin, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );

			_ro->dipVerticesNum += ro_bath_begin->verticesNum;
			_ro->dipIndiciesNum += ro_bath_begin->indicesNum;

			ro_bath_begin->material = nullptr;

			ro_bath_begin->dipVerticesNum = 0;
			ro_bath_begin->dipIndiciesNum = 0;

			vbPos += ro_bath_begin->verticesNum;
			ibPos += ro_bath_begin->indicesNum;

			ro_bath_begin->verticesNum = 0;
			ro_bath_begin->indicesNum = 0;

			++m_debugInfo.batch;
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_testRenderBB( RenderObject * _begin, RenderObject * _end, RenderObject * _ro )
	{
		for( RenderObject * it = _begin; it != _end; ++it )
		{
			RenderObject * ro_bath = it;

			if( ro_bath->verticesNum == 0 )
			{
				continue;
			}

			if( mt::is_intersect( ro_bath->bb, _ro->bb ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::batchRenderObjectSmart_( TArrayRenderObject::iterator _begin, TArrayRenderObject::iterator _end, RenderObject * _ro, RenderVertex2D * _vertexBuffer, RenderIndices2D * _indicesBuffer, size_t _vbSize, size_t _ibSize, size_t & _vbPos, size_t & _ibPos )
	{
		size_t vbPos = _vbPos;
		size_t ibPos = _ibPos;

		TArrayRenderObject::iterator it_batch_start_end = _begin;
		++it_batch_start_end;

		TArrayRenderObject::iterator it_batch_start = _begin;
		++it_batch_start;	

		for( ; it_batch_start != _end; ++it_batch_start )
		{
			RenderObject * ro_bath_start = it_batch_start;

			if( ro_bath_start->verticesNum == 0 )
			{
				continue;
			}

			if( _ro->material != ro_bath_start->material )
			{
				continue;
			}
			
			if( s_testRenderBB( it_batch_start_end, it_batch_start, ro_bath_start ) == true )
			{
				break;
			}

			//it_batch_start_end = it_batch_end;			

			this->insertRenderObject_( ro_bath_start, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );

			_ro->dipVerticesNum += ro_bath_start->verticesNum;
			_ro->dipIndiciesNum += ro_bath_start->indicesNum;

			ro_bath_start->material = nullptr;

			ro_bath_start->dipVerticesNum = 0;
			ro_bath_start->dipIndiciesNum = 0;

			vbPos += ro_bath_start->verticesNum;
			ibPos += ro_bath_start->indicesNum;

			ro_bath_start->verticesNum = 0;
			ro_bath_start->indicesNum = 0;

			++m_debugInfo.batch;
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderObjects_( RenderPass * _renderPass, RenderVertex2D * _vertexBuffer, RenderIndices2D * _indicesBuffer, size_t _vbSize, size_t _ibSize, size_t & _vbPos, size_t & _ibPos )
	{
		size_t vbPos = _vbPos;
		size_t ibPos = _ibPos;

		TArrayRenderObject::iterator it = m_renderObjects.advance( _renderPass->beginRenderObject );
		TArrayRenderObject::iterator it_end = m_renderObjects.advance( _renderPass->beginRenderObject + _renderPass->countRenderObject );
				
		for( ; it != it_end; ++it )
		{
			RenderObject * ro = it;

			if( ro->verticesNum == 0 )
			{
				continue;
			}
			
			ro->startIndex = ibPos;
			ro->minIndex = vbPos;

			this->insertRenderObject_( ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
			
			ro->dipVerticesNum = ro->verticesNum;
			ro->dipIndiciesNum = ro->indicesNum;

			vbPos += ro->verticesNum;
			ibPos += ro->indicesNum;

			ro->verticesNum = 0;
			ro->indicesNum = 0;

			switch( m_batchMode )
			{
			case ERBM_NORMAL:
				{
					this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
				}break;
			case ERBM_SMART:
				{
					this->batchRenderObjectNormal_( it, it_end, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );

					if( _renderPass->orthogonalProjection == true )
					{
						this->batchRenderObjectSmart_( it, it_end, ro, _vertexBuffer, _indicesBuffer, _vbSize, _ibSize, vbPos, ibPos );
					}
				}break;
			}	
		}

		_vbPos = vbPos;
		_ibPos = ibPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::insertRenderObject_( const RenderObject * _renderObject, RenderVertex2D * _vertexBuffer, RenderIndices2D * _indicesBuffer, size_t _vbSize, size_t _ibSize, size_t _vbPos, size_t _ibPos ) const
	{   
		if( stdex::memorycopy_safe( _vertexBuffer, _vbPos * sizeof(RenderVertex2D), _vbSize * sizeof(RenderVertex2D), _renderObject->vertexData, _renderObject->verticesNum * sizeof(RenderVertex2D) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::insertRenderObject_ vertex buffer overrlow!"
				);

			return;
		}
		//std::copy( _renderObject->vertexData, _renderObject->vertexData + _renderObject->verticesNum, offsetVertexBuffer );

		if( _ibPos > _ibSize )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::insertRenderObject_ indices buffer overrlow!"
				);
			
			return;
		}

		RenderIndices2D * offsetIndicesBuffer = _indicesBuffer + _ibPos;

		RenderIndices2D * src = offsetIndicesBuffer;
		RenderIndices2D * src_end = offsetIndicesBuffer + _renderObject->indicesNum;
		const RenderIndices2D * dst = _renderObject->indicesData;

		RenderIndices2D indices_offset = (RenderIndices2D)_vbPos;
		while( src != src_end )
		{
			*src = *dst + indices_offset;
			++src;
			++dst;
		}

		//_renderObject->baseVertexIndex = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::flushRender_()
	{
		m_debugInfo.batch = 0;
		if( this->makeBatches_() == false )
		{
			return;
		}

		m_debugInfo.dips = 0;		
		this->renderPasses_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::prepare2D_()
	{
		if( m_currentVBHandle != m_vbHandle2D )
		{
			m_currentVBHandle = m_vbHandle2D;

			RENDER_SYSTEM(m_serviceProvider)
				->setVertexBuffer( m_currentVBHandle );
		}

		if( m_currentVertexDeclaration != Vertex2D_declaration )
		{
			m_currentVertexDeclaration = Vertex2D_declaration;

			RENDER_SYSTEM(m_serviceProvider)
				->setVertexDeclaration( sizeof(RenderVertex2D), m_currentVertexDeclaration );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::recreate2DBuffers_()
	{
		if( m_ibHandle2D != 0 )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->releaseIndexBuffer( m_ibHandle2D );

			if( m_currentIBHandle == m_ibHandle2D )
			{
				m_currentIBHandle = 0;
			}

			m_ibHandle2D = 0;
		}

		m_ibHandle2D = RENDER_SYSTEM(m_serviceProvider)
			->createIndexBuffer( m_maxIndexCount, true );

		if( m_ibHandle2D == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		if( m_vbHandle2D != 0 )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->releaseVertexBuffer( m_vbHandle2D );

			if( m_currentVBHandle == m_vbHandle2D )
			{
				m_currentVBHandle = 0;
			}

			m_vbHandle2D = 0;
		}

		m_vbHandle2D = RENDER_SYSTEM(m_serviceProvider)
			->createVertexBuffer( m_maxVertexCount, sizeof(RenderVertex2D), true );

		if( m_vbHandle2D == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("RenderEngine::recreate2DBuffers_: can't create index buffer for %d indicies"
				, m_maxIndexCount 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderSystemDefaults_()
	{
		m_currentBlendSrc = BF_ONE;
		m_currentBlendDst = BF_ZERO;
		m_depthBufferWriteEnable = false;
		m_alphaBlendEnable = false;
		m_alphaTestEnable = false;

		for( int i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			RenderTextureStage & stage = m_currentTextureStage[i];

			stage.addressU = TAM_CLAMP;
			stage.addressV = TAM_CLAMP;

			stage.colorOp = TOP_DISABLE;
			stage.colorArg1 = TARG_TEXTURE;
			stage.colorArg2 = TARG_DIFFUSE;

			stage.alphaOp = TOP_DISABLE;
			stage.colorArg1 = TARG_TEXTURE;
			stage.colorArg2 = TARG_DIFFUSE;
		}

		std::fill_n( m_currentTexturesID, MENGE_MAX_TEXTURE_STAGES, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setVSync( bool _vSync )
	{
		if( m_vsync == _vSync )
		{
			return;
		}

		m_vsync = _vSync;

		if( m_windowCreated == true )
		{
			RENDER_SYSTEM(m_serviceProvider)
				->setVSync( m_vsync );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::getVSync() const
	{
		return m_vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::clear( uint32_t _color )
	{
		RENDER_SYSTEM(m_serviceProvider)
			->clear( _color );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderTargetTexture( const RenderTextureInterfacePtr & _texture, bool _clear )
	{
		const RenderImageInterfacePtr & image = _texture->getImage();

		RENDER_SYSTEM(m_serviceProvider)
			->setRenderTarget( image, _clear );

		this->restoreRenderSystemStates_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setSeparateAlphaBlendMode()
	{
		RENDER_SYSTEM(m_serviceProvider)
			->setSeparateAlphaBlendMode();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::enableDebugMode( bool _enable )
	{
		m_debugMode = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	static double s_calcTriangleSquare( const RenderVertex2D & _v1, const RenderVertex2D & _v2, const RenderVertex2D & _v3 )
	{
		const mt::vec3f & p1 = _v1.pos;
		const mt::vec3f & p2 = _v2.pos;
		const mt::vec3f & p3 = _v3.pos;

		double a = (double)mt::length_v3_v3( p1, p2 );
		double b = (double)mt::length_v3_v3( p2, p3 );
		double c = (double)mt::length_v3_v3( p3, p1 );

		double p = (a + b + c) * 0.5;

		double sq_p = sqrt(p);
		double sq_pa = sqrt(abs(p - a));
		double sq_pb = sqrt(abs(p - b));
		double sq_pc = sqrt(abs(p - c));

		double S = sq_p * sq_pa * sq_pb * sq_pc;

		return S;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcQuadSquare_( const RenderVertex2D * _vertex, size_t _num )
	{
		for( size_t i = 0; i != (_num / 4); ++i )
		{
			const RenderVertex2D & v0 = _vertex[i*4 + 0];
			const RenderVertex2D & v1 = _vertex[i*4 + 1];
			const RenderVertex2D & v2 = _vertex[i*4 + 2];
			const RenderVertex2D & v3 = _vertex[i*4 + 3];

			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v1, v2 );
			m_debugInfo.fillrate += s_calcTriangleSquare( v0, v2, v3 );

			m_debugInfo.triangle += 2;
		}        

		m_debugInfo.object += 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::calcMeshSquare_( const RenderVertex2D * _vertex, size_t _verteNum, const RenderIndices2D * _indices, size_t _indicesNum )
	{
		(void)_verteNum;

		for( size_t i = 0; i != (_indicesNum / 3); ++i )
		{
			RenderIndices2D i0 = _indices[i + 0];
			RenderIndices2D i1 = _indices[i + 1];
			RenderIndices2D i2 = _indices[i + 2];

			m_debugInfo.fillrate += s_calcTriangleSquare( _vertex[i0], _vertex[i1], _vertex[i2] );

			m_debugInfo.triangle += 1;
		}

		m_debugInfo.object += 1;
	}
}
