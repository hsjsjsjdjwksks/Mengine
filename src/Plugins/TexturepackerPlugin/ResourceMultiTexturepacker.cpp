#include "ResourceMultiTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMultiTexturepacker::ResourceMultiTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMultiTexturepacker::~ResourceMultiTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMultiTexturepacker::addResourceTexturepackerName( const ConstString & _resourceTexturepackerName )
    {
        m_resourceTexturepackerNames.emplace_back( _resourceTexturepackerName );
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorConstString & ResourceMultiTexturepacker::getResourceTexturepackerNames() const
    {
        return m_resourceTexturepackerNames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::findFrame( const ConstString & _name, ResourceImagePtr * _resourceImage ) const
    {
        ResourceImagePtr resourceImage;

        for( const ResourcePtr & resource : m_resourceTexturepackers )
        {
            UnknownResourceTexturepackerInterface * unknownResourceTexturepacker = resource->getUnknown();

            if( unknownResourceTexturepacker->hasFrame( _name, &resourceImage ) == false )
            {
                continue;
            }
        }

        if( resourceImage == nullptr )
        {
            return false;
        }

        if( _resourceImage != nullptr )
        {
            *_resourceImage = resourceImage;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMultiTexturepacker::_compile()
    {
        ResourceBankInterface * resourceBank = this->getResourceBank();

        for( const ConstString & resourceTexturepackerName : m_resourceTexturepackerNames )
        {
            const ResourcePtr & resourceTexturepacker = resourceBank->getResource( resourceTexturepackerName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceTexturepacker, false, "'%s' group '%s' invalid get texturepacker resource '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , resourceTexturepackerName.c_str()
            );

            if( resourceTexturepacker->compile() == false )
            {
                return false;
            }

            m_resourceTexturepackers.emplace_back( resourceTexturepacker );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMultiTexturepacker::_release()
    {
        for( const ResourcePtr & resourceTexturepacker : m_resourceTexturepackers )
        {
            resourceTexturepacker->release();
        }

        m_resourceTexturepackers.clear();
    }
}