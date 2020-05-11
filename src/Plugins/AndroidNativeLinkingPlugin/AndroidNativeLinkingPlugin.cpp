#include "AndroidNativeLinkingPlugin.h"

#include "AndroidNativeLinkingModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeLinking, Mengine::AndroidNativeLinkingPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLinkingPlugin::AndroidNativeLinkingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLinkingPlugin::~AndroidNativeLinkingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLinkingPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidLinking" )
            , Helper::makeModuleFactory<AndroidNativeLinkingModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLinkingPlugin::_finalizePlugin()
    {
    }
}