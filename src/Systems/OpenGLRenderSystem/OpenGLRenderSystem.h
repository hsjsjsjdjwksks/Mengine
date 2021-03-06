#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/OpenGLRenderSystemExtensionInterface.h"
#include "Interface/PlatformInterface.h"

#include "OpenGLRenderImage.h"
#include "OpenGLRenderImageTarget.h"
#include "OpenGLRenderTargetTexture.h"
#include "OpenGLRenderVertexAttribute.h"
#include "OpenGLRenderVertexBuffer.h"
#include "OpenGLRenderIndexBuffer.h"
#include "OpenGLRenderProgram.h"
#include "OpenGLRenderProgramVariable.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderSystem
        : public ServiceBase<RenderSystemInterface>
        , public OpenGLRenderSystemExtensionInterface
    {
    public:
        OpenGLRenderSystem();
        ~OpenGLRenderSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ERenderPlatform getRenderPlatformType() const override;
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _depth, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) override;

    public:
        void setViewMatrix( const mt::mat4f & _view ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;
        void setProjectionMatrix( const mt::mat4f & _projection ) override;

    public:
        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

    public:
        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex, uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount ) override;

    public:
        void setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
        void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) override;
        void setTextureFactor( uint32_t _color ) override;
        void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op ) override;
        void setCullMode( ECullMode _mode ) override;
        void setDepthBufferTestEnable( bool _depthTest ) override;
        void setDepthBufferWriteEnable( bool _depthWrite ) override;
        void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;
        void setFillMode( EFillMode _mode ) override;
        void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
        void setAlphaBlendEnable( bool _alphaBlend ) override;

    public:
        void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;

    public:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc ) override;
        bool setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _programVariable ) override;

    public:
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const DocumentPtr & _doc ) override;

        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const DocumentPtr & _doc ) override;

        RenderImageInterfacePtr createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc ) override;

    public:
        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

        bool supportTextureFormat( EPixelFormat _format ) const override;
        bool supportTextureNonPow2() const override;
        uint32_t getMaxCombinedTextureImageUnits() const override;

        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        bool onWindowChangeFullscreen( bool _fullscreen ) override;

        void setVSync( bool _vSync ) override;

        uint32_t getTextureMemoryUse() const override;
        uint32_t getTextureCount() const override;

    public:
        UnknownPointer getRenderSystemExtention() override;

    protected:
        void findFormatFromChannels_( EPixelFormat _format, uint32_t _channels, EPixelFormat * const _hwFormat, uint32_t * const _hwChannels ) const;

    protected:
        void onRenderVertexBufferDestroy_( OpenGLRenderVertexBuffer * _buffer );
        void onRenderIndexBufferDestroy_( OpenGLRenderIndexBuffer * _buffer );
        void onRenderImageDestroy_( OpenGLRenderImage * _image );
        void onRenderImageTargetDestroy_( OpenGLRenderImageTarget * _image );
        void onRenderTargetTextureDestroy_( OpenGLRenderTargetTexture * _renderTarget );
        void onRenderVertexShaderDestroy_( OpenGLRenderVertexShader * _vertexShader );
        void onRenderFragmentShaderDestroy_( OpenGLRenderFragmentShader * _fragmentShader );
        void onRenderProgramDestroy_( OpenGLRenderProgram * _program );

    protected:
        void updatePMWMatrix_();

    private:
        ConstString m_renderPlatform;

        Viewport m_viewport;

        mt::mat4f m_worldMatrix;
        mt::mat4f m_viewMatrix;
        mt::mat4f m_projectionMatrix;
        mt::mat4f m_totalWVPMatrix;

        Resolution m_windowResolution;
        Viewport m_windowViewport;

        OpenGLRenderProgramPtr m_currentProgram;
        OpenGLRenderProgramVariablePtr m_currentProgramVariable;
        OpenGLRenderVertexAttributePtr m_currentVertexAttribute;
        OpenGLRenderIndexBufferPtr m_currentIndexBuffer;
        OpenGLRenderVertexBufferPtr m_currentVertexBuffer;

        typedef Vector<OpenGLRenderVertexShaderPtr> VectorRenderVertexShaders;
        VectorRenderVertexShaders m_deferredCompileVertexShaders;

        typedef Vector<OpenGLRenderFragmentShaderPtr> VectorRenderFragmentShaders;
        VectorRenderFragmentShaders m_deferredCompileFragmentShaders;

        typedef Vector<OpenGLRenderProgramPtr> VectorDeferredRenderPrograms;
        VectorDeferredRenderPrograms m_deferredCompilePrograms;

        uint32_t m_glMaxCombinedTextureImageUnits;

        struct TextureStage
        {
            RenderImageInterface * texture = nullptr;

            GLenum minFilter = 0;
            GLenum magFilter = 0;
            GLenum wrapS = 0;
            GLenum wrapT = 0;

            uint32_t border = 0;
        };

        TextureStage m_textureStage[MENGINE_MAX_TEXTURE_STAGES];

        typedef Vector<OpenGLRenderIndexBuffer *> VectorCacheRenderIndexBuffers;
        VectorCacheRenderIndexBuffers m_cacheRenderIndexBuffers;

        typedef Vector<OpenGLRenderVertexBuffer *> VectorCacheRenderVertexBuffers;
        VectorCacheRenderVertexBuffers m_cacheRenderVertexBuffers;

        typedef Vector<OpenGLRenderImage *> VectorCacheRenderImages;
        VectorCacheRenderImages m_cacheRenderImages;

        typedef Vector<OpenGLRenderImageTarget *> VectorCacheRenderImageTargets;
        VectorCacheRenderImageTargets m_cacheRenderImageTargets;

        typedef Vector<OpenGLRenderTargetTexture *> VectorCacheRenderTargetTextures;
        VectorCacheRenderTargetTextures m_cacheRenderTargetTextures;

        typedef Vector<OpenGLRenderVertexShader *> VectorCacheRenderVertexShaders;
        VectorCacheRenderVertexShaders m_cacheRenderVertexShaders;

        typedef Vector<OpenGLRenderFragmentShader *> VectorCacheRenderFragmentShaders;
        VectorCacheRenderFragmentShaders m_cacheRenderFragmentShaders;

        typedef Vector<OpenGLRenderProgram *> VectorCacheRenderPrograms;
        VectorCacheRenderPrograms m_cacheRenderPrograms;

        bool m_renderWindowCreate;
        bool m_depthMask;

        Color m_clearColor;

        FactoryPtr m_factoryRenderVertexBuffer;
        FactoryPtr m_factoryRenderIndexBuffer;
        FactoryPtr m_factoryRenderImage;
        FactoryPtr m_factoryRenderImageTarget;
        FactoryPtr m_factoryRenderTargetTexture;
        FactoryPtr m_factoryRenderVertexAttribute;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryRenderProgram;
        FactoryPtr m_factoryRenderProgramVariable;

#ifdef MENGINE_RENDER_OPENGL
        GLuint m_vertexArrayId;
#endif
    };
}
