#include "SettingsService.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SettingsService, Mengine::SettingsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SettingsService::SettingsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SettingsService::~SettingsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SettingsService::_finalizeService()
    {
        m_settings.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::loadSetting( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc )
    {
        ConstString filePathExt = Helper::getFilePathExt( _filePath );

        PrototypeGeneratorInterfacePtr generator;
        if( PROTOTYPE_SERVICE()
            ->hasGenerator( STRINGIZE_STRING_LOCAL( "Setting" ), filePathExt, &generator ) == false )
        {
            LOGGER_ERROR( "setting '%s' file '%s' not found ext '%s' generator"
                , _name.c_str()
                , _filePath.c_str()
                , filePathExt.c_str()
            );

            return false;
        }

        SettingInterfacePtr setting = generator->generate( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( setting, false );

        if( setting->loadSetting( _fileGroup, _filePath, _doc ) == false )
        {
            return false;
        }

        m_settings.emplace( _name, setting );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SettingsService::loadSettings( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc )
    {
        LOGGER_INFO( "Settings load... %s:%s"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->loadConfig( _fileGroup, _filePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( config, false, "invalid load settings '%s'"
            , _filePath.c_str()
        );

        VectorString settingSettings;
        config->getValues( "GAME_SETTINGS", "Setting", settingSettings );

        for( const String & setting : settingSettings )
        {
            LOGGER_MESSAGE( "Setting '%s'"
                , setting.c_str()
            );

            if( config->hasSection( setting.c_str() ) == false )
            {
                LOGGER_CRITICAL( "invalid load '%s' settings no found section for '%s'"
                    , _filePath.c_str()
                    , setting.c_str()
                );

                return false;
            }

            ConstString name;
            FilePath filePath;
            config->hasValue( setting.c_str(), "Name", &name );
            config->hasValue( setting.c_str(), "Path", &filePath );

            if( this->loadSetting( name, _fileGroup, filePath, _doc ) == false )
            {
                LOGGER_CRITICAL( "invalid load setting '%s' file '%s'"
                    , name.c_str()
                    , filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const SettingInterfacePtr & SettingsService::getSetting( const ConstString & _name ) const
    {
        const SettingInterfacePtr & setting = m_settings.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( setting, SettingInterfacePtr::none() );

        return setting;
    }
    //////////////////////////////////////////////////////////////////////////
    SettingInterfacePtr SettingsService::removeSetting( const ConstString & _name )
    {
        SettingInterfacePtr setting_remove = m_settings.erase( _name );

        return setting_remove;
    }
    //////////////////////////////////////////////////////////////////////////
    void SettingsService::visitSettings( const LambdaSettings & _lambda ) const
    {
        for( const HashtableSettings::value_type & value : m_settings )
        {
            _lambda( value.key, value.element );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}