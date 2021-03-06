#include "GOAPPlugin.h"

#include "Interface/AllocatorServiceInterface.h"

#include "GOAP/Allocator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( GOAPService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( GOAP, Mengine::GOAPPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GOAPPlugin::GOAPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GOAPPlugin::~GOAPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GOAPPlugin::_initializePlugin()
    {
        SERVICE_CREATE( GOAPService, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GOAPPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( GOAPService );
    }
    //////////////////////////////////////////////////////////////////////////
    void GOAPPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( GOAPService );
    }
}