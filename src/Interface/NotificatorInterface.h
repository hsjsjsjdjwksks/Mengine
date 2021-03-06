#pragma once

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Tuple.h"
#include "Kernel/ConstString.h"
#include "Kernel/Factory.h"
#include "Kernel/Factorable.h"
#include "Kernel/Resolution.h"
#include "Kernel/LoggerLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

#ifndef MENGINE_NOTIFICATOR_MAX_COUNT
#define MENGINE_NOTIFICATOR_MAX_COUNT 256
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    typedef IntrusivePtr<class Scene, class Node> ScenePtr;
    //////////////////////////////////////////////////////////////////////////
    template<uint32_t ID>
    struct Notificator;
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_DECLARE_BEGIN()\
    static constexpr uint32_t MENGINE_NOTIFICATOR_ENUMERATOR_BEGIN = MENGINE_CODE_LINE
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_DECLARE_NOTIFICATOR(NAME, ...)\
    static constexpr uint32_t NAME = MENGINE_CODE_LINE - MENGINE_NOTIFICATOR_ENUMERATOR_BEGIN;\
    template<> struct Notificator<NAME> { typedef Tuple<__VA_ARGS__> args_type; }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_DECLARE_BEGIN();
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEBUG_OPEN_FILE, const Char *, const Char *, bool );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEBUG_CLOSE_FILE, const Char *, const Char *, bool );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEBUG_FACTORY_CREATE_OBJECT, Factory *, Factorable *, const DocumentPtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEBUG_FACTORY_DESTROY_OBJECT, Factory *, Factorable * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ASSERTION, uint32_t, const Char *, const Char *, int32_t, const Char * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ABORT );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CRASH );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ERROR, uint32_t, const Char *, int32_t, const Char * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, bool, Resolution );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CREATE_RENDER_WINDOW );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RENDER_SCENE_BEGIN, const RenderPipelineInterfacePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RENDER_SCENE_END, const RenderPipelineInterfacePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEVELOPMENT_RESOURCE_CREATE, const ResourcePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, Resource * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, Resource * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, const ScenePtr &, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_DESTROY, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_ENABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_ERROR, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_SCENE_COMPLETE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_DISABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_INITIALIZE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_ERROR, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_ENABLE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RESTART_SCENE_COMPLETE, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, const ScenePtr & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_REMOVE_SCENE_DESTROY );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_REMOVE_SCENE_COMPLETE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_LOCALE_PREPARE, const ConstString &, const ConstString & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_LOCALE, const ConstString &, const ConstString & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_LOCALE_POST, const ConstString &, const ConstString & );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_DEBUG_TEXT_MODE, bool );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_CHANGE_TEXT_ALIAS, ConstString, ConstString );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RELOAD_LOCALE_PREPARE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RELOAD_LOCALE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_RELOAD_LOCALE_POST );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ENGINE_FINALIZE );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ENGINE_ENABLE_PACKAGES );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_ENGINE_TEXTURE_DESTROY, RenderTextureInterface * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_SCRIPT_EMBEDDING );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_SCRIPT_EMBEDDING_END );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_SCRIPT_EJECTING );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_TURN_SOUND, bool );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_BOOTSTRAPPER_RUN_FRAMEWORKS );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_MODULE_INITIALIZE, ConstString );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_MODULE_FINALIZE, ConstString );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_PLUGIN_INITIALIZE, const Char * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_PLUGIN_FINALIZE, const Char * );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_INCREF_FACTORY_GENERATION, uint32_t  );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_LOGGER_BEGIN, ELoggerLevel );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_LOGGER_END, ELoggerLevel );
    MENGINE_DECLARE_NOTIFICATOR( NOTIFICATOR_TIME_FACTOR_CHANGE, float );
    //////////////////////////////////////////////////////////////////////////
#undef MENGINE_DECLARE_BEGIN
#undef MENGINE_DECLARE_NOTIFICATOR
    //////////////////////////////////////////////////////////////////////////
}
