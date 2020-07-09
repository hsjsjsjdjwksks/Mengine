#include "Movie2Data.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::Movie2Data()
        : m_movieData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::~Movie2Data()
    {
        if( m_movieData != nullptr )
        {
            ae_delete_movie_data( m_movieData );
            m_movieData = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __ae_movie_layer_data_visitor_acquire( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _ud )
    {
        AE_UNUSED( _compositionData );

        Movie2Data * data = reinterpret_cast<Movie2Data *>(_ud);

        aeMovieResourceTypeEnum resource_type = ae_get_movie_layer_data_resource_type( _layer );

        ae_userdata_t resource_ud = ae_get_movie_layer_data_resource_userdata( _layer );

        if( resource_ud == AE_USERDATA_NULL )
        {
            return AE_TRUE;
        }

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                Movie2Data::ImageDesc * image_desc = reinterpret_cast<Movie2Data::ImageDesc *>(resource_ud);

                const ResourceImagePtr & resource_image = image_desc->resourceImage;

                if( resource_image->compile() == false )
                {
                    return AE_FALSE;
                }

                if( ++image_desc->refcount == 1 )
                {
                    image_desc->materials[EMB_NORMAL] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_NORMAL, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( data ) );
                    image_desc->materials[EMB_ADD] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_ADD, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( data ) );
                    image_desc->materials[EMB_SCREEN] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_SCREEN, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( data ) );
                    image_desc->materials[EMB_MULTIPLY] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_MULTIPLY, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( data ) );
                }

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
        case AE_MOVIE_RESOURCE_SOUND:
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                Resource * resource = reinterpret_cast<Resource *>(resource_ud);

                if( resource->compile() == false )
                {
                    return AE_FALSE;
                }

                return AE_TRUE;
            }break;
        default:
            break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquireComposition( const aeMovieComposition * _composition )
    {
        if( ae_visit_nodes_layer_data( _composition, &__ae_movie_layer_data_visitor_acquire, this ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __ae_movie_layer_data_visitor_release( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _ud )
    {
        AE_UNUSED( _compositionData );
        AE_UNUSED( _ud );

        aeMovieResourceTypeEnum resource_type = ae_get_movie_layer_data_resource_type( _layer );

        ae_userdata_t resource_ud = ae_get_movie_layer_data_resource_userdata( _layer );

        if( resource_ud == AE_USERDATA_NULL )
        {
            return AE_TRUE;
        }

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                Movie2Data::ImageDesc * image_desc = reinterpret_cast<Movie2Data::ImageDesc *>(resource_ud);

                const ResourceImagePtr & resource_image = image_desc->resourceImage;

                resource_image->release();

                if( --image_desc->refcount == 0 )
                {
                    image_desc->materials[EMB_NORMAL] = nullptr;
                    image_desc->materials[EMB_ADD] = nullptr;
                    image_desc->materials[EMB_SCREEN] = nullptr;
                    image_desc->materials[EMB_MULTIPLY] = nullptr;
                }

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
        case AE_MOVIE_RESOURCE_SOUND:
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                Resource * resource = reinterpret_cast<Resource *>(resource_ud);

                resource->release();

                return AE_TRUE;
            }break;
        default:
            break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::releaseComposition( const aeMovieComposition * _composition )
    {
        ae_visit_nodes_layer_data( _composition, &__ae_movie_layer_data_visitor_release, this );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer Movie2Data::allocateMemory( size_t _size ) const
    {
        MENGINE_UNUSED( _size );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::setMovieData( const aeMovieData * _movieData )
    {
        m_movieData = _movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieData * Movie2Data::getMovieData() const
    {
        return m_movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Movie2Data::getResource( const ae_string_t _resourceName )
    {
        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResourceReference( Helper::stringizeString( _resourceName ) );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "not found resource '%s'"
            , _resourceName
        );

        m_resources.emplace_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::ImageDesc * Movie2Data::makeImageDesc( const ResourceImagePtr & _resource )
    {
        ImageDesc * desc = m_poolImageDesc.createT();

        desc->refcount = 0;
        desc->resourceImage = _resource;

        m_images.push_back( desc );

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::removeImageDesc( ImageDesc * _desc )
    {
        m_poolImageDesc.destroyT( _desc );
    }
}