#include "Movie2.h"

#include "Interface/TimelineInterface.h"
#include "Interface/NodeInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PrototypeManagerInterface.h"
#include "Interface/ResourceInterface.h"

#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceVideo.h"
#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceTrackMatte.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"

#include "Kernel/Materialable.h"

#include "Logger/Logger.h"

#include "math/quat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2::Movie2()
        : m_composition( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::~Movie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie2 )
    {
        if( m_resourceMovie2 == _resourceMovie2 )
        {
            return;
        }

        m_resourceMovie2 = _resourceMovie2;
        
        this->recompile();
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceMovie2Ptr & Movie2::getResourceMovie2() const
    {
        return m_resourceMovie2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setCompositionName( const ConstString & _compositionName )
    {
        m_compositionName = _compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2::getCompositionName() const
    {
        return m_compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::getDuration() const
    {
        float duration = ae_get_movie_composition_duration( m_composition );

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setWorkAreaFromEvent( const ConstString & _eventName )
    {
        float a, b;
        ae_bool_t ok = ae_get_movie_composition_node_in_out_time( m_composition, _eventName.c_str(), AE_MOVIE_LAYER_TYPE_EVENT, &a, &b );

        if( ok == AE_FALSE )
        {
            return;
        }

        ae_set_movie_composition_work_area( m_composition, a, b );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeWorkArea()
    {
        ae_remove_movie_composition_work_area( m_composition );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::playSubComposition( const ConstString & _name )
    {
        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( m_composition, _name.c_str() );

        ae_play_movie_sub_composition( m_composition, subcomposition, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::stopSubComposition( const ConstString & _name )
    {
        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( m_composition, _name.c_str() );

        ae_stop_movie_sub_composition( m_composition, subcomposition );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::interruptSubComposition( const ConstString & _name, bool _skip )
    {
        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( m_composition, _name.c_str() );

        ae_interrupt_movie_sub_composition( m_composition, subcomposition, _skip );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setLoopSubComposition( const ConstString & _name, bool _loop )
    {
        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( m_composition, _name.c_str() );

        ae_set_movie_sub_composition_loop( subcomposition, _loop );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setEnableMovieLayers( const ConstString & _name, bool _enable )
    {
        if( m_composition == nullptr )
        {
            LOGGER_ERROR("Movie2::setEnableMovieLayers '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , _name.c_str()
                );

            return;
        }

        ae_set_movie_composition_nodes_enable_any( m_composition, _name.c_str(), _enable ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_play( float _time )
    {
        (void)_time;

        if( this->isCompile() == false )
        {
            return false;
        }

        ae_play_movie_composition( m_composition, 0.f );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_restart( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_RESTART )
            ->onAnimatableRestart( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_pause( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_PAUSE )
            ->onAnimatablePause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_resume( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_RESUME )
            ->onAnimatableResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_stop( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_STOP )
            ->onAnimatableStop( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * __movie_composition_camera_provider( const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * movie2 = (Movie2 *)_data;

        ConstString c_name = Helper::stringizeString( _callbackData->name );

        Movie2::Camera * old_camera;
        if( movie2->getCamera( c_name, &old_camera ) == true )
        {
            return old_camera;
        }

        RenderCameraProjection * renderCameraProjection = NODE_SERVICE()
            ->createNodeT<RenderCameraProjection *>( STRINGIZE_STRING_LOCAL( "RenderCameraProjection" ) );

        renderCameraProjection->setName( c_name );

        mt::vec3f cameraTarget;
        cameraTarget.from_f3( _callbackData->target );
        mt::vec3f cameraPosition;
        cameraPosition.from_f3( _callbackData->position );

        mt::vec3f cameraDirection;
        mt::norm_v3_v3( cameraDirection, cameraTarget - cameraPosition );

        float aspect = _callbackData->width / _callbackData->height;

        renderCameraProjection->setCameraPosition( cameraPosition );
        renderCameraProjection->setCameraDirection( cameraDirection );
        renderCameraProjection->setCameraFOV( _callbackData->fov );
        renderCameraProjection->setCameraAspect( aspect );

        RenderViewport * renderViewport = NODE_SERVICE()
            ->createNodeT<RenderViewport *>( STRINGIZE_STRING_LOCAL( "RenderViewport" ) );

        renderViewport->setName( c_name );

        Viewport vp;
        vp.begin.x = 0.f;
        vp.begin.y = 0.f;

        vp.end.x = _callbackData->width;
        vp.end.y = _callbackData->height;

        renderViewport->setViewport( vp );

        Movie2::Camera * new_camera = movie2->addCamera( c_name, renderCameraProjection, renderViewport );

        return new_camera;
    }
    //////////////////////////////////////////////////////////////////////////
    static void __movie_composition_camera_deleter( const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * movie2 = (Movie2 *)_data;

        ConstString c_name = Helper::stringizeString( _callbackData->name );

        movie2->removeCamera( c_name );
    }
    //////////////////////////////////////////////////////////////////////////
    static void __movie_composition_camera_update( const aeMovieCameraUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_data;

        Movie2::Camera * camera = (Movie2::Camera *)(_callbackData->camera_data);

        //camera
        mt::vec3f cameraTarget;
        cameraTarget.from_f3( _callbackData->target );
        mt::vec3f cameraPosition;
        cameraPosition.from_f3( _callbackData->position );

        mt::vec3f cameraDirection;
        mt::norm_v3_v3( cameraDirection, cameraTarget - cameraPosition );

        camera->projection->setCameraPosition( cameraPosition );
        camera->projection->setCameraDirection( cameraDirection );
    }
    //////////////////////////////////////////////////////////////////////////
    static EMaterialBlendMode getMovieBlendMode( ae_blend_mode_t _ae_blend_mode )
    {
        EMaterialBlendMode blend_mode = EMB_NORMAL;

        switch( _ae_blend_mode )
        {
        case AE_MOVIE_BLEND_ADD:
            blend_mode = EMB_ADD;
            break;
        case AE_MOVIE_BLEND_SCREEN:
            blend_mode = EMB_SCREEN;
            break;
        case AE_MOVIE_BLEND_MULTIPLY:
            blend_mode = EMB_MULTIPLY;
            break;
        default:
            break;
        };

        return blend_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    static EMaterialBlendMode getMovieLayerBlendMode( const aeMovieLayerData * _layer )
    {
        ae_blend_mode_t layer_blend_mode = ae_get_movie_layer_data_blend_mode( _layer );

        EMaterialBlendMode blend_mode = getMovieBlendMode( layer_blend_mode );

        return blend_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * __movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * movie2 = (Movie2 *)_data;

        const aeMovieLayerData * layer = _callbackData->layer;

        const char * layer_name = ae_get_movie_layer_data_name( layer );

        ConstString c_name = Helper::stringizeString( layer_name );

        ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( layer );

        if( is_track_matte == AE_TRUE )
        {
            return AE_NULL;
        }

        aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( layer );

        switch( type )
        {
#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
        case AE_MOVIE_LAYER_TYPE_SPRITE:
            {
                ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                    ->getResourceReferenceT<ResourceImagePtr>( c_name );

                if( resourceImage == nullptr )
                {
                    LOGGER_ERROR( "Movie2 '%s' resource '%s' composition '%s' layer '%s' invalid get resource for image %s"
                        , movie2->getName().c_str()
                        , movie2->getResourceMovie2()->getName().c_str()
                        , movie2->getCompositionName().c_str()
                        , c_name.c_str()
                        , c_name.c_str()
                    );

                    return AE_NULL;
                }

                SurfaceImagePtr surface = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ) );

                if( surface == nullptr )
                {
                    return AE_NULL;
                }

                EMaterialBlendMode blend_mode = getMovieLayerBlendMode( layer );

                surface->setBlendMode( blend_mode );

                surface->setResourceImage( resourceImage );

                ShapeQuadFixed * node = NODE_SERVICE()
                    ->createNodeT<ShapeQuadFixed *>( STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ) );
                
                if( node == nullptr )
                {
                    LOGGER_ERROR( "Movie::createMovieSprite_ '%s' resource '%s' composition '%s' layer '%s' invalid create 'Sprite'"
                        , movie2->getName().c_str()
                        , movie2->getResourceMovie2()->getName().c_str()
                        , movie2->getCompositionName().c_str()
                        , c_name.c_str()
                    );

                    return AE_NULL;
                }

                node->setName( c_name );
                node->setExternalRender( true );

                node->setSurface( surface );
                                
                MatrixProxy * matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ) );

                matrixProxy->setName( c_name );

                mt::mat4f pm;
                pm.from_f16( _callbackData->matrix );
                matrixProxy->setProxyMatrix( pm );
                matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                matrixProxy->addChild( node );

                movie2->addChild( matrixProxy );

                movie2->addMatrixProxy( matrixProxy );

                return node;
            }break;
#endif
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                TextField * node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ) );

                node->setName( c_name );
                node->setExternalRender( true );

                node->setTextID( c_name );
                
                if( ae_has_movie_layer_data_param( layer, AE_MOVIE_LAYER_PARAM_HORIZONTAL_CENTER ) == AE_TRUE )
                {
                    node->setHorizontalCenterAlign();
                }

                if( ae_has_movie_layer_data_param( layer, AE_MOVIE_LAYER_PARAM_VERTICAL_CENTER ) == AE_TRUE )
                {
                    node->setVerticalCenterAlign();
                }
                
                movie2->addText( c_name, node );

                MatrixProxy * matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ) );

                matrixProxy->setName( c_name );

                mt::mat4f pm;
                pm.from_f16( _callbackData->matrix );
                matrixProxy->setProxyMatrix( pm );
                matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                
                matrixProxy->addChild( node );
                
                movie2->addChild( matrixProxy );

                movie2->addMatrixProxy( matrixProxy );

                return node;
            }break;
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                Movie2Slot * node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );

                node->setName( c_name );
                node->setExternalRender( true );

                const ConstString & movie2Name = movie2->getName();

                node->setMovieName( movie2Name );

                movie2->addSlot( c_name, node );

                MatrixProxy * matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ) );

                matrixProxy->setName( c_name );

                mt::mat4f pm;
                pm.from_f16( _callbackData->matrix );
                matrixProxy->setProxyMatrix( pm );
                matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                matrixProxy->addChild( node );

                movie2->addChild( matrixProxy );

                movie2->addMatrixProxy( matrixProxy );

                return node;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOCKET:
            {
                HotSpotPolygon * node = NODE_SERVICE()
                    ->createNodeT<HotSpotPolygon *>( STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );

                node->setName( c_name );
                node->setExternalRender( true );

                const ae_polygon_t * polygon;
                if( ae_get_movie_layer_data_socket_polygon( _callbackData->layer, 0, &polygon ) == AE_FALSE )
                {
                    return AE_NULL;
                }

                Polygon p;
                for( ae_uint32_t index = 0; index != polygon->point_count; ++index )
                {
                    const ae_vector2_t * v = polygon->points + index;

                    mt::vec2f v2( (*v)[0], (*v)[1] );
                    
                    p.append( v2 );
                }

                node->setPolygon( p );

                movie2->addSocketShape( c_name, node );

                MatrixProxy * matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ) );

                matrixProxy->setName( c_name );

                mt::mat4f pm;
                pm.from_f16( _callbackData->matrix );
                matrixProxy->setProxyMatrix( pm );
                matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                matrixProxy->addChild( node );

                movie2->addChild( matrixProxy );

                movie2->addMatrixProxy( matrixProxy );

                return node;
            }break;
        default:
            break;
        };

        if( _callbackData->track_matte_layer != AE_NULL )
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    SurfaceTrackMatte * surfaceTrackMatte = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceTrackMatte" ) );

                    surfaceTrackMatte->setName( c_name );

                    ResourceImage * resourceImage = (ResourceImage *)ae_get_movie_layer_data_resource_data( _callbackData->layer );
                    ResourceImage * resourceTrackMatteImage = (ResourceImage *)ae_get_movie_layer_data_resource_data( _callbackData->track_matte_layer );

                    surfaceTrackMatte->setResourceImage( resourceImage );
                    surfaceTrackMatte->setResourceTrackMatteImage( resourceTrackMatteImage );

                    ae_track_matte_mode_t track_matte_mode = ae_get_movie_layer_data_track_matte_mode( _callbackData->layer );
                    
                    switch( track_matte_mode )
                    {
                    case AE_MOVIE_TRACK_MATTE_ALPHA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA_INVERTED );
                        }break;
                    default:
                        break;
                    }                    

                    EMaterialBlendMode blend_mode = getMovieLayerBlendMode( layer );

                    surfaceTrackMatte->setBlendMode( blend_mode );
                    
                    movie2->addSurface( surfaceTrackMatte );

                    return surfaceTrackMatte;
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
                {
                    ParticleEmitter2 * node = NODE_SERVICE()
                        ->createNodeT<ParticleEmitter2 *>( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

                    node->setName( c_name );
                    node->setExternalRender( true );

                    ResourceParticle * resourceParticle = (ResourceParticle *)ae_get_movie_layer_data_resource_data( _callbackData->layer );

                    node->setResourceParticle( resourceParticle );

                    //EMaterialBlendMode blend_mode = getMovieBlendMode( layer );

                    ae_float_t layer_stretch = ae_get_movie_layer_data_stretch( _callbackData->layer );
                    node->setStretch( layer_stretch );

                    node->setLoop( _callbackData->incessantly );

                    node->setEmitterPositionRelative( false );
                    node->setEmitterCameraRelative( false );
                    node->setEmitterTranslateWithParticle( false );

                    movie2->addParticle( node );

                    MatrixProxy * matrixProxy = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ) );

                    matrixProxy->setName( c_name );

                    mt::mat4f pm;
                    pm.from_f16( _callbackData->matrix );
                    matrixProxy->setProxyMatrix( pm );
                    matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                    matrixProxy->addChild( node );

                    movie2->addChild( matrixProxy );

                    movie2->addMatrixProxy( matrixProxy );

                    return node;
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    SurfaceVideo * surfaceVideo = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );

                    surfaceVideo->setName( c_name );

                    ResourceVideo * resourceVideo = (ResourceVideo *)ae_get_movie_layer_data_resource_data( _callbackData->layer );

                    surfaceVideo->setResourceVideo( resourceVideo );

                    EMaterialBlendMode blend_mode = getMovieLayerBlendMode( layer );

                    surfaceVideo->setLoop( _callbackData->incessantly );

                    surfaceVideo->setBlendMode( blend_mode );
                    surfaceVideo->setPremultiplyAlpha( true );
                    
                    movie2->addSurface( surfaceVideo );

                    return surfaceVideo;
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    SurfaceSound * surfaceSound = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSound" ) );

                    surfaceSound->setName( c_name );

                    surfaceSound->setLoop( _callbackData->incessantly );

                    ResourceSound * resourceSound = (ResourceSound *)ae_get_movie_layer_data_resource_data( _callbackData->layer );

                    surfaceSound->setResourceSound( resourceSound );
                    
                    movie2->addSurface( surfaceSound );

                    return surfaceSound;
                }break;
            default:
                break;
            }
        }

        return AE_NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_node_deleter( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * movie2 = (Movie2 *)_data;
        (void)movie2;

        ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( _callbackData->layer );

        if( is_track_matte == AE_TRUE )
        {
            return;
        }

        aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( _callbackData->layer );

        if( _callbackData->track_matte_layer != AE_NULL )
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    //SurfaceTrackMatte * surfaceTrackMatte = (SurfaceTrackMatte *)_callbackData->element;
                    
                    //movie2->removeSurface( surfaceTrackMatte );
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
                {
                    //ParticleEmitter2 * particleEmitter = (ParticleEmitter2 *)_callbackData->element;

                    //movie2->removeParticle( particleEmitter );
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    //SurfaceVideo * surfaceVideo = (SurfaceVideo *)_callbackData->element;

                    //movie2->removeSurface( surfaceVideo );
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    //SurfaceSound * surfaceSound = (SurfaceSound *)_callbackData->element;

                    //movie2->removeSurface( surfaceSound );
                }break;
            default:
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_node_update( const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_data;
            
        aeMovieLayerTypeEnum type = _callbackData->type;

        switch( _callbackData->state )
        {
        case AE_MOVIE_STATE_UPDATE_PROCESS:
            {
                switch( type )
                {
#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = (ShapeQuadFixed *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                    }break;
#endif
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = (TextField *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * node = (ParticleEmitter2 *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = (Movie2Slot *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>( nodeParent );

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                        HotSpotPolygon * node = (HotSpotPolygon *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );
                    }break;
                default:
                    break;
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_BEGIN:
            {
                switch( type )
                {
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * node = (ParticleEmitter2 *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        node->setTiming( _callbackData->offset * 1000.f );

                        if( _callbackData->loop == AE_TRUE )
                        {
                            if( node->isPlay() == false )
                            {
                                if( node->play( time ) == 0 )
                                {
                                    return;
                                }
                            }
                        }
                        else
                        {
                            if( node->play( time ) == 0 )
                            {
                                return;
                            }
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        SurfaceVideo * node = (SurfaceVideo *)_callbackData->element;

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        node->setTiming( _callbackData->offset * 1000.f );

                        if( _callbackData->loop == AE_TRUE )
                        {
                            if( node->isPlay() == false )
                            {
                                if( node->play( time ) == 0 )
                                {
                                    return;
                                }
                            }
                        }
                        else
                        {
                            if( node->play( time ) == 0 )
                            {
                                return;
                            }
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOUND:
                    {
                        SurfaceSound * node = (SurfaceSound *)_callbackData->element;

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        node->setTiming( _callbackData->offset * 1000.f );

                        if( _callbackData->loop == AE_TRUE )
                        {
                            if( node->isPlay() == false )
                            {
                                if( node->play( time ) == 0 )
                                {
                                    return;
                                }
                            }
                        }
                        else
                        {
                            if( node->play( time ) == 0 )
                            {
                                return;
                            }
                        }
                    }break;
                default:
                    break;
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_END:
            {
                switch( type )
                {
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * node = (ParticleEmitter2 *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                        if( _callbackData->loop == AE_FALSE )
                        {
                            node->stop();
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        SurfaceVideo * animatable = (SurfaceVideo *)_callbackData->element;

                        if( _callbackData->loop == AE_FALSE )
                        {
                            animatable->stop();
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOUND:
                    {
                        SurfaceSound * animatable = (SurfaceSound *)_callbackData->element;

                        if( _callbackData->loop == AE_FALSE )
                        {
                            animatable->stop();
                        }
                    }break;
                default:
                    break;
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_PAUSE:
            {
                switch( type )
                {
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * node = (ParticleEmitter2 *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                        node->pause();
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        SurfaceVideo * animatable = (SurfaceVideo *)_callbackData->element;

                        animatable->pause();
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOUND:
                    {
                        SurfaceSound * animatable = (SurfaceSound *)_callbackData->element;

                        animatable->pause();
                    }break;
                default:
                    break;
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_RESUME:
            {
                switch( type )
                {
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * node = (ParticleEmitter2 *)_callbackData->element;

                        Node * nodeParent = node->getParent();

                        MatrixProxy * matrixProxy = static_node_cast<MatrixProxy *>(nodeParent);

                        mt::mat4f mp;
                        mp.from_f16( _callbackData->matrix );
                        matrixProxy->setProxyMatrix( mp );

                        matrixProxy->setLocalColorRGBA( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        node->resume( time );
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        SurfaceVideo * animatable = (SurfaceVideo *)_callbackData->element;

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        animatable->resume( time );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOUND:
                    {
                        SurfaceSound * animatable = (SurfaceSound *)_callbackData->element;

                        float time = TIMELINE_SERVICE()
                            ->getTime();

                        animatable->resume( time );
                    }break;
                default:
                    break;
                }
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    struct TrackMatteDesc
    {
        mt::mat4f matrix;
        aeMovieRenderMesh mesh;
        ae_track_matte_mode_t mode;
    };
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t __movie_composition_track_matte_provider( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_data;

        TrackMatteDesc * desc = new TrackMatteDesc;

        desc->matrix.from_f16( _callbackData->matrix );
        desc->mesh = *_callbackData->mesh;
        desc->mode = _callbackData->track_matte_mode;

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_track_matte_update( const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_data;

        switch( _callbackData->state )
        {
        case AE_MOVIE_STATE_UPDATE_BEGIN:
            {
                TrackMatteDesc * desc = static_cast<TrackMatteDesc *>(_callbackData->track_matte_data);

                desc->matrix.from_f16( _callbackData->matrix );
                desc->mesh = *_callbackData->mesh;
            }break;
        case AE_MOVIE_STATE_UPDATE_PROCESS:
            {
                TrackMatteDesc * desc = static_cast<TrackMatteDesc *>(_callbackData->track_matte_data);

                desc->matrix.from_f16( _callbackData->matrix );
                desc->mesh = *_callbackData->mesh;
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_track_matte_deleter( const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_data;

        TrackMatteDesc * desc = (TrackMatteDesc *)_callbackData->element;

        delete desc;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t __movie_composition_shader_provider( const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_callbackData;
        (void)_data;

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_shader_property_update( const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_callbackData;
        (void)_data;

    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_event( const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _data )
    {
        (void)_callbackData;
        (void)_data;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_state( const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        if( _callbackData->subcomposition != AE_NULL )
        {
            return;
        }

        Movie2 * m2 = (Movie2 *)(_data);

        if( _callbackData->state == AE_MOVIE_COMPOSITION_END )
        {
            m2->stop();
        }
    }
#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t __movie_scene_effect_provider( const aeMovieCompositionSceneEffectProviderCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * m2 = (Movie2 *)(_data);

        Node * parent = m2->getParent();

        mt::vec3f anchor_point;
        anchor_point.from_f2( _callbackData->anchor_point );
        parent->setOrigin( anchor_point );

        mt::vec3f position;
        position.from_f2( _callbackData->position );
        parent->setLocalPosition( position );

        mt::vec3f scale;
        scale.from_f2( _callbackData->scale );
        parent->setScale( scale );

        mt::quatf q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = _callbackData->quaternion[0];
        q.w = _callbackData->quaternion[1];
        float angle = quatzw_to_angle( q );

        parent->setOrientationX( angle );

        return AE_NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_scene_effect_update( const aeMovieCompositionSceneEffectUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * m2 = (Movie2 *)(_data);

        Node * parent = m2->getParent();

        mt::vec3f anchor_point;
        anchor_point.from_f2( _callbackData->anchor_point );
        parent->setOrigin( anchor_point );

        mt::vec3f position;
        position.from_f2( _callbackData->position );
        parent->setLocalPosition( position );

        mt::vec3f scale;
        scale.from_f2( _callbackData->scale );
        parent->setScale( scale );

        mt::quatf q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = _callbackData->quaternion[0];
        q.w = _callbackData->quaternion[1];
        float angle = quatzw_to_angle( q );

        parent->setOrientationX( angle );
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    Movie2::Camera * Movie2::addCamera( const ConstString & _name, RenderCameraProjection * _projection, RenderViewport * _viewport )
    {
        this->addChild( _projection );
        this->addChild( _viewport );

        Camera c;
        c.projection = _projection;
        c.viewport = _viewport;

        TMapCamera::iterator it_found = m_cameras.insert( std::make_pair( _name, c ) ).first;

        Camera * new_camera = &it_found->second;

        return new_camera;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::removeCamera( const ConstString & _name )
    {
        TMapCamera::iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        Camera & c = it_found->second;

        c.projection->destroy();
        c.viewport->destroy();

        m_cameras.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasCamera( const ConstString & _name ) const
    {
        TMapCamera::const_iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::getCamera( const ConstString & _name, Camera ** _camera )
    {
        TMapCamera::iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        Camera & camera = it_found->second;

        *_camera = &camera;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_compile()
    {
        if( m_resourceMovie2 == nullptr )
        {
            LOGGER_ERROR( "Movie2::_compile: '%s' can't setup resource"
                , this->getName().c_str()
                );

            return false;
        }

        if( m_resourceMovie2.compile() == false )
        {
            LOGGER_ERROR( "Movie2::_compile '%s' resource %s not compile"
                , m_name.c_str()
                , m_resourceMovie2->getName().c_str()
                );

            return false;
        }

        const aeMovieData * movieData = m_resourceMovie2->getMovieData();

        const aeMovieCompositionData * compositionData = m_resourceMovie2->getCompositionData( m_compositionName );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR("Movie2::_compile '%s' resource %s not found composition '%s'"
                , m_name.c_str()
                , m_resourceMovie2->getName().c_str()
                , m_compositionName.c_str()
                );

            return false;
        }

        aeMovieCompositionProviders providers;
        ae_initialize_movie_composition_providers( &providers );

        providers.camera_provider = &__movie_composition_camera_provider;
        providers.camera_deleter = &__movie_composition_camera_deleter;
        providers.camera_update = &__movie_composition_camera_update;

        providers.node_provider = &__movie_composition_node_provider;
        providers.node_deleter = &__movie_composition_node_deleter;
        providers.node_update = &__movie_composition_node_update;

        providers.track_matte_provider = &__movie_composition_track_matte_provider;
        providers.track_matte_update = &__movie_composition_track_matte_update;
        providers.track_matte_deleter = &__movie_composition_track_matte_deleter;

        providers.shader_provider = &__movie_composition_shader_provider;
        providers.shader_property_update = &__movie_composition_shader_property_update;

        providers.composition_event = &__movie_composition_event;
        providers.composition_state = &__movie_composition_state;

#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
        providers.scene_effect_provider = &__movie_scene_effect_provider;
        providers.scene_effect_update = &__movie_scene_effect_update;
#endif

        aeMovieComposition * composition = ae_create_movie_composition( movieData, compositionData, AE_TRUE, &providers, this );

        if( composition == nullptr )
        {
            return false;
        }

        aeMovieCompositionRenderInfo info;
        ae_calculate_movie_composition_render_info( composition, &info );

        m_meshes.resize( info.max_render_node );

        bool loop = this->getLoop();

        ae_set_movie_composition_loop( composition, loop ? AE_TRUE : AE_FALSE );

        bool play = this->isPlay();

        if( play == true )
        {
            float timing = this->getTiming();

            ae_play_movie_composition( composition, timing * 0.001f );
        }

        m_composition = composition;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_release()
    {
        ae_delete_movie_composition( m_composition );
        m_composition = nullptr;

        m_meshes.clear();
        m_cameras.clear();

        for( TVectorSurfaces::const_iterator
            it = m_surfaces.begin(),
            it_end = m_surfaces.end();
            it != it_end;
            ++it )
        {
            const SurfacePtr & surface = *it;

            surface->release();
        }

        m_surfaces.clear();
        
        for( TMapSlots::iterator
            it = m_slots.begin(),
            it_end = m_slots.end();
            it != it_end;
            ++it )
        {
            Movie2Slot * slot = it->second;

            slot->destroy();
        }

        m_slots.clear();

        for( TMapSockets::iterator
            it = m_sockets.begin(),
            it_end = m_sockets.end();
            it != it_end;
            ++it )
        {
            HotSpot * hotspot = it->second;

            hotspot->destroy();
        }

        m_sockets.clear();

        for( TVectorMatrixProxies::iterator
            it = m_matrixProxies.begin(),
            it_end = m_matrixProxies.end();
            it != it_end;
            ++it )
        {
            MatrixProxy * matrixProxy = *it;

            matrixProxy->destroy();
        }

        m_matrixProxies.clear();

        m_resourceMovie2.release();

        Node::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        ae_vector3_t anchorPoint;
        if( ae_get_movie_composition_anchor_point( m_composition, anchorPoint ) == AE_TRUE )
        {
            mt::vec3f origin;

            origin.from_f3( anchorPoint );

            this->setOrigin( origin );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_deactivate()
    {
        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_changeParent( Node * _oldParent, Node * _newParent )
    {
        (void)_oldParent;
        (void)_newParent;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_addChild( Node * _node )
    {
        (void)_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_removeChild( Node * _node )
    {
        (void)_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_afterActivate()
    {
        Node::_afterActivate();

        bool autoPlay = this->getAutoPlay();

        if( autoPlay == true )
        {
            float time = TIMELINE_SERVICE()
                ->getTime();

            if( this->play( time ) == 0 )
            {
                LOGGER_ERROR( "Movie2::_afterActivate '%s' resource '%s' auto play return 0"
                    , this->getName().c_str()
                    , this->m_resourceMovie2->getName().c_str()
                    );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        ae_set_movie_composition_loop( m_composition, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setTiming( float _time )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        ae_set_movie_composition_time( m_composition, _time * 0.001f );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::_getTiming() const
    {
        if( this->isCompile() == false )
        {
            return 0.f;
        }

        float timing = ae_get_movie_composition_time( m_composition );

        return timing * 1000.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setFirstFrame()
    {
        this->setTiming( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLastFrame()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "Movie._setLastFrame: '%s' not activate"
                , this->getName().c_str()
                );

            return;
        }

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_composition_data( m_composition );

        float duration = ae_get_movie_composition_data_duration( compositionData );
        float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );
        
        this->setTiming( duration * 1000.f - frameDuration * 1000.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_update( float _current, float _timing )
    {
        (void)_current;
        (void)_timing;

        if( this->isPlay() == false )
        {
            return;
        }

        if( m_composition == nullptr )
        {
            return;
        }
        
        ae_update_movie_composition( m_composition, _timing * 0.001f );

        for( TVectorSurfaces::const_iterator
            it = m_surfaces.begin(),
            it_end = m_surfaces.end();
            it != it_end;
            ++it )
        {
            const SurfacePtr & surface = *it;

            surface->update( _current, _timing  );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_render( Mengine::RenderServiceInterface * _renderService, const RenderState * _state )
    {
        uint32_t mesh_iterator = 0;
        Mesh * meshes_buffer = &m_meshes.front();

        const mt::mat4f & wm = this->getWorldMatrix();

        ColourValue total_color;
        this->calcTotalColor( total_color );

        float total_color_r = total_color.getR();
        float total_color_g = total_color.getG();
        float total_color_b = total_color.getB();
        float total_color_a = total_color.getA();

        ae_voidptr_t composition_camera_data = ae_get_movie_composition_camera_data( m_composition );
        (void)composition_camera_data;

        ae_uint32_t compute_movie_mesh_iterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &compute_movie_mesh_iterator, &mesh ) == AE_TRUE )
        {
            ResourceReference * resource_reference = (ResourceReference *)mesh.resource_data;

            RenderState state;

            if( mesh.camera_data != nullptr )
            {
                Movie2::Camera * camera = (Movie2::Camera *)mesh.camera_data;

                state.camera = camera->projection;
                state.viewport = camera->viewport;
                state.clipplane = _state->clipplane;
                state.target = _state->target;
            }
            else
            {
                state.camera = _state->camera;
                state.viewport = _state->viewport;
                state.clipplane = _state->clipplane;
                state.target = _state->target;
            }

            ColourValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r, total_color_g * mesh.color.g, total_color_b * mesh.color.b, total_color_a * mesh.opacity );

            if( mesh.track_matte_data == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = reinterpret_cast<Movie2Slot *>(mesh.element_data);
                        
                        node->render( _renderService, _state, 0 );
                    }break;
#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = reinterpret_cast<ShapeQuadFixed *>(mesh.element_data);

                        node->render( _renderService, _state, 0 );

                    }break;
#endif
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = reinterpret_cast<TextField *>(mesh.element_data);

                        node->render( _renderService, _state, 0 );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        ParticleEmitter2 * particleEmitter2 = reinterpret_cast<ParticleEmitter2 *>(mesh.element_data);

                        particleEmitter2->render( _renderService, _state, 0 );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                    {
                        Mesh & m = meshes_buffer[mesh_iterator++];

                        m.vertices.resize( mesh.vertexCount );
                                                
                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = m.vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            v.uv[0] = uv;
                            v.uv[1] = uv;

                            v.color = total_mesh_color;
                        }

                        m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

                        EMaterialBlendMode blend_mode = getMovieBlendMode( mesh.blend_mode );

                        m.material = Helper::makeTextureMaterial( nullptr, 0, ConstString::none(), blend_mode, false, false, false );

                        _renderService
                            ->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOLID:
                    {
                        Mesh & m = meshes_buffer[mesh_iterator++];

                        m.vertices.resize( mesh.vertexCount );

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = m.vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            v.uv[0] = uv;
                            v.uv[1] = uv;

                            v.color = total_mesh_color;
                        }

                        m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

                        EMaterialBlendMode blend_mode = getMovieBlendMode( mesh.blend_mode );

                        m.material = Helper::makeTextureMaterial( nullptr, 0, ConstString::none(), blend_mode, false, false, false );

                        _renderService
                            ->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        ResourceImage * resource_image = static_cast<ResourceImage *>(resource_reference);

                        Mesh & m = meshes_buffer[mesh_iterator++];

                        m.vertices.resize( mesh.vertexCount );

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = m.vertices[index];

                            mt::vec3f vp;
                            const float * vp3 = mesh.position[index];
                            vp.from_f3( vp3 );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            const float * uv2 = mesh.uv[index];
                            uv.from_f2( uv2 );

                            resource_image->correctUVImage( v.uv[0], uv );
                            resource_image->correctUVAlpha( v.uv[1], uv );

                            v.color = total_mesh_color;
                        }

                        m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

                        EMaterialBlendMode blend_mode = getMovieBlendMode( mesh.blend_mode );

                        m.material = Helper::makeImageMaterial( resource_image, ConstString::none(), blend_mode, false, false );

                        if( m.vertices.size() == 0 )
                        {
                            continue;
                        }

                        _renderService
                            ->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        SurfaceVideo * surfaceVideo = static_cast<SurfaceVideo *>(mesh.element_data);

                        Mesh & m = meshes_buffer[mesh_iterator++];

                        m.vertices.resize( mesh.vertexCount );

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = m.vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            surfaceVideo->correctUV( 0, v.uv[0], uv );
                            surfaceVideo->correctUV( 1, v.uv[1], uv );

                            v.color = total_mesh_color;
                        }

                        m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

                        m.material = surfaceVideo->getMaterial();

                        _renderService
                            ->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
                    }break;
                default:
                    break;
                }
            }
            else
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        const SurfaceTrackMatte * surfaceTrackMatte = static_cast<const SurfaceTrackMatte *>(mesh.element_data);

                        if( surfaceTrackMatte == nullptr )
                        {
                            return;
                        }

                        Mesh & m = meshes_buffer[mesh_iterator++];

                        m.vertices.resize( mesh.vertexCount );

                        const ResourceImagePtr & resourceImage = surfaceTrackMatte->getResourceImage();
                        const ResourceImagePtr & resourceTrackMatteImage = surfaceTrackMatte->getResourceTrackMatteImage();

                        const TrackMatteDesc * track_matte_desc = static_cast<const TrackMatteDesc *>(mesh.track_matte_data);

                        const aeMovieRenderMesh * track_matte_mesh = &track_matte_desc->mesh;

                        const RenderTextureInterfacePtr & texture_trackmatte = resourceTrackMatteImage->getTexture();
                        const mt::uv4f & texture_trackmatte_uv = texture_trackmatte->getUV();

                        for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = m.vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( mesh.position[index] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );
                            
                            resourceImage->correctUVImage( v.uv[0], uv );

                            mt::vec2f uv_track_matte;
                            uv_track_matte = calc_point_uv(
                                mt::vec2f( track_matte_mesh->position[0] ), mt::vec2f( track_matte_mesh->position[1] ), mt::vec2f( track_matte_mesh->position[2] ),
                                mt::vec2f( track_matte_mesh->uv[0] ), mt::vec2f( track_matte_mesh->uv[1] ), mt::vec2f( track_matte_mesh->uv[2] ),
                                vp.to_vec2f()
                            );
                            
                            mt::multiply_tetragon_uv4_v2( v.uv[1], texture_trackmatte_uv, uv_track_matte );

                            v.color = total_mesh_color;
                        }

                        m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

                        //EMaterialBlendMode blend_mode = getMovieBlendMode( mesh.blend_mode );

                        m.material = surfaceTrackMatte->getMaterial();

                        if( m.vertices.size() == 0 )
                        {
                            continue;
                        }

                        _renderService
                            ->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
                    }break;
                default:
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_interrupt( uint32_t _enumerator )
    {
        (void)_enumerator;

        ae_interrupt_movie_composition( m_composition, AE_FALSE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSurface( const SurfacePtr & _surface )
    {
        _surface->compile();

        m_surfaces.push_back( _surface );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeSurface( const SurfacePtr & _surface )
    {
        _surface->release();

        TVectorSurfaces::iterator it_found = std::find( m_surfaces.begin(), m_surfaces.end(), _surface );

        m_surfaces.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addParticle( ParticleEmitter2 * _particleEmitter )
    {
        m_particleEmitters.push_back( _particleEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeParticle( ParticleEmitter2 * _particleEmitter )
    {
        _particleEmitter->release();

        TVectorParticleEmitter2s::iterator it_found = std::find( m_particleEmitters.begin(), m_particleEmitters.end(), _particleEmitter );

        m_particleEmitters.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSlot( const ConstString & _name, Movie2Slot * _slot )
    {
        m_slots.insert( std::make_pair( _name, _slot ) );
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Slot * Movie2::getSlot( const ConstString & _name ) const
    {
        TMapSlots::const_iterator it_found = m_slots.find( _name );

        if( it_found == m_slots.end() )
        {
            return nullptr;
        }

        Movie2Slot * slot = it_found->second;

        return slot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSlot( const ConstString & _name ) const
    {
        TMapSlots::const_iterator it_found = m_slots.find( _name );

        if( it_found == m_slots.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSocketShape( const ConstString & _name, HotSpotPolygon * _hotspot )
    {
        m_sockets.insert( std::make_pair( _name, _hotspot ) );
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpot * Movie2::getSocket( const ConstString & _name ) const
    {
        TMapSockets::const_iterator it_found = m_sockets.find( _name );

        if( it_found == m_sockets.end() )
        {
            return nullptr;
        }

        HotSpot * hotspot = it_found->second;

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSocket( const ConstString & _name ) const
    {
        TMapSockets::const_iterator it_found = m_sockets.find( _name );

        if( it_found == m_sockets.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addText( const ConstString & _name, TextField * _text )
    {
        m_texts.insert( std::make_pair( _name, _text ) );
    }
    //////////////////////////////////////////////////////////////////////////
    TextField * Movie2::getText( const ConstString & _name ) const
    {
        TMapTexts::const_iterator it_found = m_texts.find( _name );

        if( it_found == m_texts.end() )
        {
            return nullptr;
        }

        TextField * text = it_found->second;

        return text;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasText( const ConstString & _name ) const
    {
        TMapTexts::const_iterator it_found = m_texts.find( _name );

        if( it_found == m_texts.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addMatrixProxy( MatrixProxy * _matrixProxy )
    {
        m_matrixProxies.push_back( _matrixProxy );
    }
}
