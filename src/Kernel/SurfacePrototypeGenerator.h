#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Kernel/Surface.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class SurfacePrototypeGenerator
        : public ScriptablePrototypeGenerator<Type, Count>
    {
    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            SurfacePtr surface = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_MESSAGE( _doc )
            );

            const ConstString & prototype = this->getPrototype();
            surface->setType( prototype );

#ifdef MENGINE_DEBUG
            DocumentPtr doc = MENGINE_DOCUMENT( "Surface '%s' type '%s' create '%s'"
                , surface->getName().c_str()
                , surface->getType().c_str()
                , MENGINE_DOCUMENT_MESSAGE( _doc )
            );

            surface->setDocument( doc );
#endif

            this->setupScriptable( surface );

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            return surface;
        }
    };
}