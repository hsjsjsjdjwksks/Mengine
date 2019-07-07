#include "Config/Config.h"

#include "SDLApplication.h"

#include "Engine/Application.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/EasingServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include <cstdio>
#include <clocale>

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"

#include "Kernel/StringArguments.h"
#include "Kernel/FileLogger.h"
#include "Kernel/IniUtil.h"
#include "Kernel/Document.h"

#include "SDLMessageBoxLogger.h"

#ifdef MENGINE_PLATFORM_ANDROID
#   include "AndroidLogger.h"
#else
#   include "SDLStdioLogger.h"
#endif

#include <ctime>
#include <algorithm>

#include <sstream>
#include <iomanip>

#include "SDL_filesystem.h"

#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif


//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( Application );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );

SERVICE_EXTERN( ArchiveService );

SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );

SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( RenderMaterialService );

SERVICE_EXTERN( UnicodeSystem );

SERVICE_EXTERN( FileService );

SERVICE_EXTERN( NotificationService );

SERVICE_EXTERN( SoundSystem );
SERVICE_EXTERN( SilentSoundSystem );
SERVICE_EXTERN( SoundService );

SERVICE_EXTERN( InputService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( VocabularyService );

SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( UpdateService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( WatchdogService );
SERVICE_EXTERN( GraveyardService );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( EasingService );
SERVICE_EXTERN( AccountService );
SERVICE_EXTERN( SceneService );
SERVICE_EXTERN( ChronometerService );
SERVICE_EXTERN( PickerService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( BitmapFont );
PLUGIN_EXPORT( DebugRender );
PLUGIN_EXPORT( ResourceValidate );

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
PLUGIN_EXPORT( MetabufLoader );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
PLUGIN_EXPORT( Video );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
PLUGIN_EXPORT( Theora );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
PLUGIN_EXPORT( TTF );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
PLUGIN_EXPORT( cURL );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
PLUGIN_EXPORT( Spine );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
PLUGIN_EXPORT( ResourcePrefetcher );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
PLUGIN_EXPORT( Movie );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
PLUGIN_EXPORT( Movie1 );
#endif

//PLUGIN_EXPORT( Box2D );
PLUGIN_EXPORT( OggVorbis );
//PLUGIN_EXPORT( PathFinder );
PLUGIN_EXPORT( SDLFileGroup );

#ifdef MENGINE_PLUGIN_ASTRALAX_STATIC
PLUGIN_EXPORT( AstralaxParticlePlugin2 );
#endif

#ifdef MENGINE_PLUGIN_STEAM_STATIC
PLUGIN_EXPORT( Steam );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
PLUGIN_EXPORT( NodeDebugger );
#endif

#ifdef MENGINE_PLUGIN_POSIX_SOCKET_STATIC
PLUGIN_EXPORT( PosixSocket );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK_STATIC
PLUGIN_EXPORT( AndroidNativeFacebook );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_STATIC
PLUGIN_EXPORT( AndroidNativeUnityAds );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_STATIC
PLUGIN_EXPORT( AndroidNativeAdMob );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV_STATIC
PLUGIN_EXPORT( AndroidNativeDevToDev );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
PLUGIN_EXPORT( PythonFramework );
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
    {
        _mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
    {
        _mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::SDLApplication()
        : m_serviceProvider( nullptr )
        , m_running( false )
        , m_active( false )
        , m_developmentMode( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::~SDLApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeFileEngine_()
    {
        LOGGER_INFO( "Inititalizing File Service..." );
        SERVICE_CREATE( FileService );

        LOGGER_INFO( "Initialize SDL file group..." );
        PLUGIN_CREATE( SDLFileGroup );

        // mount root
        ConstString c_dir = Helper::stringizeString( "dir" );
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, FilePath( ConstString::none() ), c_dir, nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory"
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        ConstString c_dev = Helper::stringizeString( "dev" );
        // mount root
        if( FILE_SERVICE()
            ->mountFileGroup( c_dev, defaultFileGroup, FilePath( ConstString::none() ), c_dir, nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory"
            );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::loadApplicationConfig_()
    {
        FilePath applicationPath = STRINGIZE_FILEPATH_LOCAL( MENGINE_APPLICATION_INI_PATH );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, applicationPath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "Invalid open application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, stream ) == false )
        {
            LOGGER_ERROR( "Invalid load application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        const Char * publicConfigPath;
        if( IniUtil::getIniValue( ini, "Config", "Public", &publicConfigPath ) == false )
        {
            if( IniUtil::getIniValue( ini, "Game", "Path", &publicConfigPath ) == false )
            {
                LOGGER_ERROR( "Not found public config path '%s'"
                    , applicationPath.c_str()
                );

                return false;
            }
        }

        IniUtil::getIniValue( ini, "Game", "Path", m_�onfigPaths );
        IniUtil::getIniValue( ini, "Config", "Path", m_�onfigPaths );

        const Char * resourcePath;
        if( IniUtil::getIniValue( ini, "Resource", "Path", &resourcePath ) == false )
        {
            LOGGER_ERROR( "Not found resource path '%s'"
                , applicationPath.c_str()
            );

            return false;
        }

        m_resourceConfigPath = Helper::stringizeFilePath( resourcePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeConfigEngine_()
    {
        LOGGER_WARNING( "Inititalizing Config Manager..." );
        SERVICE_CREATE( ConfigService );


        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        for( const FilePath & filePath : m_�onfigPaths )
        {
            if( CONFIG_SERVICE()
                ->loadConfig( fileGroup, filePath ) == false )
            {
                LOGGER_ERROR( "invalid load config %s"
                    , filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeUserDirectory_()
    {
        Char userPath[MENGINE_MAX_PATH];
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        FilePath cs_userPath = Helper::stringizeFilePathSize( userPath, userPathLen );

        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, cs_userPath, STRINGIZE_STRING_LOCAL( "global" ), nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount user directory '%s'"
                , cs_userPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerFile_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        m_fileLog = new FactorableUnique<FileLogger>();

        LOGGER_SERVICE()
            ->registerLogger( m_fileLog );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerEngine_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

#if defined(MENGINE_PLATFORM_ANDROID)
        m_loggerStdio = new FactorableUnique<AndroidLogger>();
#else
        m_loggerStdio = new FactorableUnique<SDLStdioLogger>();
#endif

        m_loggerStdio->setVerboseFlag( LM_MESSAGE );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerStdio );

        m_loggerMessageBox = new FactorableUnique<SDLMessageBoxLogger>();

        m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerMessageBox );        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeArchiveService_()
    {
        LOGGER_INFO( "Inititalizing Archive Service..." );

        {
            LOGGER_INFO( "initialize Zip..." );

            PLUGIN_CREATE( Zip );
        }

        {
            LOGGER_INFO( "initialize LZ4..." );

            PLUGIN_CREATE( LZ4 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeRenderEngine_()
    {
        LOGGER_INFO( "Initializing Render Service..." );

        if( SERVICE_CREATE( RenderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( RenderTextureService ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( RenderMaterialService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeSoundEngine_()
    {
        LOGGER_INFO( "Initializing Sound Service..." );

        SERVICE_CREATE( SoundSystem );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true || SERVICE_EXIST( Mengine::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( SilentSoundSystem );
        }

        if( SERVICE_CREATE( SoundService ) == false )
        {
            LOGGER_ERROR( "Failed to create Sound Engine"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initialize( const int argc, char** argv )
    {
        setlocale( LC_ALL, "C" );

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( FactoryService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( UnicodeSystem );
        SERVICE_CREATE( OptionsService );

        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>();

        for( int i = 1; i < argc; ++i )
        {
            const Char * arg = argv[i];

            arguments->addArgument( arg );
        }

        if( OPTIONS_SERVICE()
            ->setArguments( arguments ) == false )
        {
            return false;
        }

        SERVICE_CREATE( NotificationService );
        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( VocabularyService );
        SERVICE_CREATE( LoggerService );

        if( this->initializeLoggerEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( Platform );
        SERVICE_CREATE( PluginService );

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }

        if( this->loadApplicationConfig_() == false )
        {
            return false;
        }

        if( this->initializeConfigEngine_() == false )
        {
            return false;
        }

        if( this->initializeUserDirectory_() == false )
        {
            return false;
        }

        if( this->initializeLoggerFile_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ArchiveService );

        if( this->initializeArchiveService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ThreadSystem );
        SERVICE_CREATE( ThreadService );

        m_mutexAllocatorPool = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
            , (stdex_allocator_thread_lock_t)&s_stdex_thread_lock
            , (stdex_allocator_thread_unlock_t)&s_stdex_thread_unlock
        );

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }

        if( this->initializeSoundEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ModuleService );
        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( PrefetcherService );        
        SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( InputService );
        SERVICE_CREATE( EnumeratorService );
        SERVICE_CREATE( TimeSystem );
        SERVICE_CREATE( PrototypeService );
        SERVICE_CREATE( UpdateService );
        SERVICE_CREATE( LoaderService );
        SERVICE_CREATE( RenderService );
        SERVICE_CREATE( RenderMaterialService );
        SERVICE_CREATE( RenderTextureService );
        SERVICE_CREATE( ChronometerService );
        SERVICE_CREATE( AccountService );
        SERVICE_CREATE( SceneService );
        SERVICE_CREATE( ResourceService );
        SERVICE_CREATE( TextService );
        SERVICE_CREATE( WatchdogService );
        SERVICE_CREATE( GraveyardService );
        SERVICE_CREATE( PackageService );
        SERVICE_CREATE( UserdataService );
        SERVICE_CREATE( PickerService );
        SERVICE_CREATE( PlayerService );
        SERVICE_CREATE( GameService );
        SERVICE_CREATE( TimelineService );
        SERVICE_CREATE( EasingService );

#define MENGINE_ADD_PLUGIN( Name, Info )\
        do{LOGGER_INFO( Info );\
        if(	PLUGIN_CREATE(Name) == false ){\
        LOGGER_ERROR( "Invalid %s", Info );}else{\
        LOGGER_WARNING( "Successful %s", Info );}}while(false, false)

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "initialize Plugin PythonFramework..." );
#endif

#ifdef MENGINE_PLUGIN_DEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( DebugRender, "initialize Plugin Debug Render..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "initialize Plugin Resource Validate..." );
#endif

        MENGINE_ADD_PLUGIN( ImageCodec, "initialize Plugin Image Codec..." );
        MENGINE_ADD_PLUGIN( SoundCodec, "initialize Plugin Sound Codec..." );
        MENGINE_ADD_PLUGIN( OggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
        MENGINE_ADD_PLUGIN( Amplifier, "initialize Plugin Amplifier..." );

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "initialize Plugin Video..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "initialize Plugin Resource Prefetcher..." );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "initialize Plugin Metabuf Loader..." );
#endif


#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "initialize Plugin Theora..." );
#endif

        MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin TTF..." );

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF..." );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "initialize Plugin cURL..." );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "initialize Plugin Spine..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "initialize Plugin Movie..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "initialize Plugin Movie1..." );
#endif

        //MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
        //MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D..." );

        //MENGINE_ADD_PLUGIN( PathFinder, "initialize Plugin Path Finder..." );

#ifdef MENGINE_PLUGIN_ASTRALAX_STATIC
        MENGINE_ADD_PLUGIN( AstralaxParticlePlugin2, "initialize Astralax Particle Plugin..." );
#endif


#ifdef MENGINE_PLUGIN_STEAM_STATIC
        MENGINE_ADD_PLUGIN( Steam, "initialize Steam Plugin..." );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "initialize Plugin NodeDebugger..." );
#endif

#ifdef MENGINE_PLUGIN_POSIX_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( PosixSocket, "initialize Plugin PosixSocket..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeFacebook, "initialize Android Facebook Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeUnityAds, "initialize Android Unity Ads Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeAdMob, "initialize Android AdMob Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeDevToDev, "initialize Android DevToDev Native..." );
#endif

#undef MENGINE_ADD_PLUGIN

        VectorString plugins;
        CONFIG_VALUES( "Plugins", "Name", plugins );

        for( const String & pluginName : plugins )
        {
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str() ) == false )
            {
                LOGGER_ERROR( "Application Failed to load plugin '%s'"
                    , pluginName.c_str()
                );

                return false;
            }
        }

#ifdef MENGINE_MASTER_RELEASE
        bool devplugins = false;
        bool devmodules = false;
#else
#ifdef MENGINE_DEBUG
        bool devplugins = true;
        bool devmodules = true;
#else
        bool developmentMode = HAS_OPTION( "dev" );
        bool devplugins = developmentMode;
        bool devmodules = developmentMode;
#endif
#endif

        bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "DevPlugins", "Name", devPlugins );

            for( const String & pluginName : devPlugins )
            {
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str() ) == false )
                {
                    LOGGER_WARNING( "Application Failed to load dev plugin '%s'"
                        , pluginName.c_str()
                    );
                }
            }
        }

        SERVICE_CREATE( Application );

        VectorString modules;
        CONFIG_VALUES( "Modules", "Name", modules );

        for( const String & moduleName : modules )
        {
            if( MODULE_SERVICE()
                ->runModule( Helper::stringizeString( moduleName ), MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_ERROR( "Application Failed to run module '%s'"
                    , moduleName.c_str()
                );

                return false;
            }
        }

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
        {
            VectorString devModules;
            CONFIG_VALUES( "DevModules", "Name", devModules );

            for( const String & moduleName : devModules )
            {
                if( MODULE_SERVICE()
                    ->runModule( Helper::stringizeString( moduleName ), MENGINE_DOCUMENT_FUNCTION ) == false )
                {
                    LOGGER_ERROR( "Application Failed to run dev module '%s'"
                        , moduleName.c_str()
                    );
                }
            }
        }

        FilePath renderMaterialsPath = CONFIG_VALUE( "Engine", "RenderMaterials", FilePath::none() );

        if( renderMaterialsPath.empty() == false )
        {
            const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
                ->getDefaultFileGroup();

            if( RENDERMATERIAL_SERVICE()
                ->loadMaterials( defaultFileGroup, renderMaterialsPath ) == false )
            {
                return false;
            }
        }

        LOGGER_INFO( "Application Create..." );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( defaultFileGroup, m_resourceConfigPath ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        FRAMEWORK_SERVICE()
            ->onFrameworkInitialize();

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Game invalid load personality"
            );

            return false;
        }

        PLATFORM_SERVICE()
            ->setIcon( 0 );

        String projectTitle;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ), &entry ) == false )
        {
            LOGGER_WARNING( "Application not setup title 'APPLICATION_TITLE'"
            );
        }
        else
        {
            projectTitle = entry->getValue();
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle.c_str() );

        const Resolution & windowResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        LOGGER_INFO( "Creating Render Window..." );

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            return false;
        }

        APPLICATION_SERVICE()
            ->turnSound( true );

        GAME_SERVICE()
            ->run();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::loop()
    {
        PLATFORM_SERVICE()
            ->runPlatform();

        PLATFORM_SERVICE()
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        SERVICE_FINALIZE( Mengine::ModuleServiceInterface );

        if( SERVICE_EXIST( Mengine::PlatformInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
        }

        if( SERVICE_EXIST( Mengine::NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        SERVICE_PROVIDER_STOP();

        if( SERVICE_EXIST( Mengine::ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        SERVICE_FINALIZE( Mengine::AccountServiceInterface );
        SERVICE_FINALIZE( Mengine::GameServiceInterface );
        SERVICE_FINALIZE( Mengine::PickerServiceInterface );
        SERVICE_FINALIZE( Mengine::SceneServiceInterface );
        SERVICE_FINALIZE( Mengine::PlayerServiceInterface );
        SERVICE_FINALIZE( Mengine::ApplicationInterface );
        SERVICE_FINALIZE( Mengine::PackageServiceInterface );
        SERVICE_FINALIZE( Mengine::UserdataServiceInterface );
        SERVICE_FINALIZE( Mengine::GraveyardInterface );
        SERVICE_FINALIZE( Mengine::ChronometerServiceInterface );
        SERVICE_FINALIZE( Mengine::UpdateServiceInterface );
        SERVICE_FINALIZE( Mengine::ResourceServiceInterface );
        SERVICE_FINALIZE( Mengine::TextServiceInterface );
        SERVICE_FINALIZE( Mengine::PrototypeServiceInterface );
        SERVICE_FINALIZE( Mengine::PrefetcherServiceInterface );
        SERVICE_FINALIZE( Mengine::DataServiceInterface );
        SERVICE_FINALIZE( Mengine::PluginServiceInterface );
        SERVICE_FINALIZE( Mengine::InputServiceInterface );
        SERVICE_FINALIZE( Mengine::UnicodeSystemInterface );

        SERVICE_FINALIZE( Mengine::CodecServiceInterface );

        SERVICE_FINALIZE( Mengine::SoundServiceInterface );
        SERVICE_FINALIZE( Mengine::SoundSystemInterface );

        SERVICE_FINALIZE( Mengine::ConverterServiceInterface );

        SERVICE_FINALIZE( Mengine::RenderServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderMaterialServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderTextureServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderSystemInterface );

        SERVICE_FINALIZE( Mengine::ConfigServiceInterface );
        SERVICE_FINALIZE( Mengine::StringizeServiceInterface );

        SERVICE_FINALIZE( Mengine::ArchiveServiceInterface );
        SERVICE_FINALIZE( Mengine::MemoryServiceInterface );
        SERVICE_FINALIZE( Mengine::NotificationServiceInterface );

        SERVICE_FINALIZE( Mengine::ThreadServiceInterface );

        SERVICE_FINALIZE( Mengine::TimeSystemInterface );

        SERVICE_FINALIZE( Mengine::EasingServiceInterface );
        SERVICE_FINALIZE( Mengine::VocabularyServiceInterface );
        SERVICE_FINALIZE( Mengine::EnumeratorServiceInterface );

        SERVICE_FINALIZE( Mengine::PlatformInterface );

        if( m_fileLog != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_fileLog );

            m_fileLog = nullptr;
        }

        if( m_loggerStdio != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerStdio );

            m_loggerStdio = nullptr;
        }

        if( m_loggerMessageBox != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerMessageBox );

            m_loggerMessageBox = nullptr;
        }

        SERVICE_FINALIZE( Mengine::FileServiceInterface );
        SERVICE_FINALIZE( Mengine::ThreadSystemInterface );
        SERVICE_FINALIZE( Mengine::NotificationServiceInterface );
        SERVICE_FINALIZE( Mengine::LoggerServiceInterface );

        stdex_allocator_finalize_threadsafe();

        m_mutexAllocatorPool = nullptr;

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );        
    }
}
