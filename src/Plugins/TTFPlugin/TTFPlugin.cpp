#include "TTFPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AllocatorHelper.h"

#include "TTFPrototypeGenerator.h"
#include "TTFFontConfigLoader.h"
#include "TTFDataflow.h"
#include "FEDataflow.h"

#include "fe/fe.h"
#include "stdex/allocator_report.h"

//////////////////////////////////////////////////////////////////////////
void * _fe_alloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "fe" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _fe_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "fe" );
}
//////////////////////////////////////////////////////////////////////////
void _debug_image_created( fe_image * )
{
}
//////////////////////////////////////////////////////////////////////////
void _debug_image_deleted( fe_image * )
{
}
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( TTFAtlasService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TTF, Mengine::TTFPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFPlugin::TTFPlugin()
        : m_ftlibrary( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFPlugin::~TTFPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFPlugin::_initializePlugin()
    {
        SERVICE_CREATE( TTFAtlasService, MENGINE_DOCUMENT_FACTORABLE );

        FT_Library ftlibrary;
        FT_Error ft_err = FT_Init_FreeType( &ftlibrary );

        if( ft_err != 0 )
        {
            LOGGER_ERROR( "invalid init FreeType '%d'"
                , ft_err
            );

            return false;
        }

        TTFPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<TTFPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        generator->setFTLibrary( ftlibrary );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ), generator ) == false )
        {
            return false;
        }

        VOCABULARY_SET( TextFontConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ), Helper::makeFactorableUnique<TTFFontConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );


        m_ftlibrary = ftlibrary;

        m_ftMutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            TTFDataflowPtr dataflowTTF = Helper::makeFactorableUnique<TTFDataflow>( MENGINE_DOCUMENT_FACTORABLE );

            dataflowTTF->setFTLibrary( m_ftlibrary );

            dataflowTTF->setMutex( m_ftMutex );

            if( dataflowTTF->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ), dataflowTTF, MENGINE_DOCUMENT_FACTORABLE );

            FEDataflowPtr dataflowFE = Helper::makeFactorableUnique<FEDataflow>( MENGINE_DOCUMENT_FACTORABLE );

            if( dataflowFE->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ), dataflowFE, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowTTF = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );
            dataflowTTF->finalize();

            DataflowInterfacePtr dataflowFE = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );
            dataflowFE->finalize();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_finalizePlugin()
    {
        m_ftMutex = nullptr;

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "TTF" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), STRINGIZE_STRING_LOCAL( "TTF" ) );

        SERVICE_FINALIZE( TTFAtlasService );

        FT_Done_FreeType( m_ftlibrary );
        m_ftlibrary = nullptr;

#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "fe" );
        MENGINE_ASSERTION( report_count == 0, "FE memleak [%d]"
            , report_count
        );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( TTFAtlasService );
    }
}
