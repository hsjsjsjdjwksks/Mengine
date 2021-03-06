#include "ZOrderRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ZOrderRender::ZOrderRender()
        : m_zOrder( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ZOrderRender::~ZOrderRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ZOrderRender::setZOrder( int32_t _zOrder )
    {
        m_zOrder = _zOrder;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t ZOrderRender::getZOrder() const
    {
        return m_zOrder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZOrderRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
}