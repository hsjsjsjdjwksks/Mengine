#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<ResourcePtr> ResourcePointer;
    //////////////////////////////////////////////////////////////////////////
    class ResourceBankInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved ) = 0;
        virtual void finalize() = 0;

    public:
        virtual ResourcePointer createResource( const ConstString & _locale
            , const ConstString & _groupName
            , const ConstString & _name
            , const ConstString & _type
            , bool _keep
            , Resource ** _prev
            , const Char * _doc ) = 0;

    public:
        virtual void removeResource( const ResourcePtr & _resource ) = 0;

    public:
        virtual void destroyResource( Resource * _resource ) = 0;

    public:
        virtual const ResourcePtr & getResource( const ConstString & _name ) const = 0;
        virtual const ResourcePtr & getResourceReference( const ConstString & _name ) const = 0;
        virtual const ConstString & getResourceType( const ConstString & _name ) const = 0;

    public:
        virtual bool hasResource( const ConstString & _name, ResourcePtr * _resource ) const = 0;

        template<class T>
        bool hasResourceT( const ConstString & _name, T * _resource ) const
        {
            ResourcePtr resource;
            if( this->hasResource( _name, &resource ) == false )
            {
                return false;
            }

#ifdef MENGINE_DEBUG
            if( resource == nullptr )
            {
                return false;
            }

            if( stdex::intrusive_dynamic_cast<T>(resource) == nullptr )
            {
                throw;
            }
#endif

            * _resource = stdex::intrusive_static_cast<T>(resource);

            return true;
        }

        virtual bool hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * _resource ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceBankInterface> ResourceBankInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
