#pragma once

#include "HotSpotPolygon.h"

#include "Kernel/Surface.h"

namespace Mengine
{
    class HotSpotSurface
        : public HotSpotPolygon
    {
        DECLARE_VISITABLE( HotSpotPolygon );

    public:
        HotSpotSurface();
        ~HotSpotSurface() override;

    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        SurfacePtr m_surface;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotSurface> HotSpotSurfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
