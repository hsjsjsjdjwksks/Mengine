#pragma once

#include "Kernel/Resource.h"

#include "Kernel/Polygon.h"

namespace Mengine
{
    class ResourceShape
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceShape();
        ~ResourceShape() override;

    public:
        const Polygon & getPolygon() const;

    public:
        bool _loader( const Metabuf::Metadata * _meta ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        Polygon m_polygon;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceShape> ResourceShapePtr;
    //////////////////////////////////////////////////////////////////////////
}