#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Eventable.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool hasEventableReceiver( const T * _self, uint32_t _event )
        {
            const EventationInterface * eventation = _self->getEventation();

            if( eventation == nullptr )
            {
                return false;
            }

            bool exist = eventation->hasEventReceiver( _event );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool hasEventableReceiver( const IntrusivePtr<T> & _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            if( eventation == nullptr )
            {
                return false;
            }

            bool exist = eventation->hasEventReceiver( _event );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getThisEventReciever( Eventable * _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * reciever = eventation->getEventRecieverT<T *>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getThisEventReciever( const EventablePtr & _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * reciever = eventation->getEventRecieverT<T *>( _event );

            return reciever;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENTABLE()\
public:\
    Mengine::EventationInterface * getEventation() override{ return this; }\
    const Mengine::EventationInterface * getEventation() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODRT(Event, R, Type)\
    Mengine::Helper::hasEventableReceiver( this, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( this, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODR(Event, R)\
    EVENTABLE_METHODRT(Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHOD(Event)\
    EVENTABLE_METHODRT(Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODR(Self, Event, R)\
    EVENTABLE_OTHER_METHODRT(Self, Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHOD(Self, Event)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODT(Self, Event, Type)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), Type)
//////////////////////////////////////////////////////////////////////////