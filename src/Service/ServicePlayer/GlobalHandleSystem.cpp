#include "GlobalHandleSystem.h"

#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GlobalHandleSystem, Mengine::GlobalHandleSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystem::GlobalHandleSystem()
        : m_handlersEnumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystem::~GlobalHandleSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GlobalHandleSystem::_finalizeService()
    {
        m_handlers.clear();
        m_handlersAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleKeyEvent( const InputKeyEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleKeyEvent( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleTextEvent( const InputTextEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleTextEvent( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseButtonEvent( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseButtonEventBegin( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseButtonEventEnd( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseMove( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        for( const GlobalHandlerDesc & desc : m_handlers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.enable == false )
            {
                continue;
            }

            desc.handler->handleMouseWheel( _event );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t GlobalHandleSystem::addGlobalHandler( const InputHandlerInterfacePtr & _handler, const String & _doc )
    {
        GlobalHandlerDesc desc;

        uint32_t new_id = ++m_handlersEnumerator;
        desc.id = new_id;

        desc.handler = _handler;
        desc.doc = _doc;
        desc.enable = true;
        desc.dead = false;

        m_handlersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    InputHandlerInterfacePtr GlobalHandleSystem::removeGlobalHandler( uint32_t _id )
    {
        VectorGlobalHandler::iterator it_found_add = std::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), [_id]( const GlobalHandlerDesc & _handle ) { return _handle.id == _id; } );

        if( it_found_add != m_handlersAdd.end() )
        {
            InputHandlerInterfacePtr handler = it_found_add->handler;
            it_found_add->handler = nullptr;

            m_handlersAdd.erase( it_found_add );

            return handler;
        }

        VectorGlobalHandler::iterator it_found = std::find_if( m_handlers.begin(), m_handlers.end(), [_id]( const GlobalHandlerDesc & _handle ) { return _handle.id == _id; } );

        if( it_found == m_handlers.end() )
        {
            LOGGER_ERROR( "GlobalHandleSystem::removeGlobalMouseEventable not found %d"
                , _id
            );

            return nullptr;
        }

        InputHandlerInterfacePtr handler = it_found->handler;

        it_found->dead = true;
        it_found->handler = nullptr;

        return handler;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class GlobalKeyHandler
            : public InputHandlerInterface
            , public Factorable
        {
        public:
            GlobalKeyHandler( KeyCode _code, const GlobalHandleSystem::LambdaKeyHandler & _handler )
                : m_code( _code )
                , m_handler( _handler )
            {
            }

            ~GlobalKeyHandler() override
            {
            }

        protected:
            bool handleKeyEvent( const InputKeyEvent & _event ) override
            {
                if( _event.code == m_code && _event.isDown == true )
                {
                    m_handler( _event );
                }

                return false;
            }

            bool handleTextEvent( const InputTextEvent & _event ) override
            {
                (void)_event;

                return false;
            }

        protected:
            bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseMove( const InputMouseMoveEvent & _event ) override
            {
                (void)_event;

                return false;
            }

            bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
            {
                (void)_event;

                return false;
            }

        protected:
            KeyCode m_code;
            GlobalHandleSystem::LambdaKeyHandler m_handler;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t GlobalHandleSystem::addGlobalKeyHandler( const String & _doc, KeyCode _code, const LambdaKeyHandler & _handler )
    {
        uint32_t id = this->addGlobalHandler( new FactorableUnique<Detail::GlobalKeyHandler>( _code, _handler ), _doc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GlobalHandleSystem::enableGlobalHandler( uint32_t _id, bool _value )
    {
        VectorGlobalHandler::iterator it_found_add = std::find_if( m_handlersAdd.begin(), m_handlersAdd.end(), [_id]( const GlobalHandlerDesc & _handle ) { return _handle.id == _id; } );

        if( it_found_add != m_handlersAdd.end() )
        {
            it_found_add->enable = _value;

            return true;
        }

        VectorGlobalHandler::iterator it_found = std::find_if( m_handlers.begin(), m_handlers.end(), [_id]( const GlobalHandlerDesc & _handle ) { return _handle.id == _id; } );

        if( it_found == m_handlers.end() )
        {
            LOGGER_ERROR( "GlobalHandleSystem::enableGlobalHandler not found %d"
                , _id
            );

            return false;
        }

        it_found->enable = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GlobalHandleSystem::update()
    {
        VectorGlobalHandler::iterator it_mouse_erase = std::remove_if( m_handlers.begin(), m_handlers.end(), []( const GlobalHandlerDesc & _handle ) {return _handle.dead; } );
        m_handlers.erase( it_mouse_erase, m_handlers.end() );

        m_handlers.insert( m_handlers.begin(), m_handlersAdd.begin(), m_handlersAdd.end() );
        m_handlersAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void GlobalHandleSystem::clear()
    {
        this->update();

        if( m_handlers.empty() == false )
        {
            LOGGER_ERROR( "GlobalHandleSystem::clear global handlers is not empty:"
            );

            for( const GlobalHandlerDesc & desc : m_handlers )
            {
                LOGGER_ERROR( "%s"
                    , desc.doc.c_str()
                );
            }
        }
    }

}