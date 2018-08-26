#include "Kernel/Affector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Affector::Affector()
        : m_type( ETA_POSITION )
        , m_id( 0 )
        , m_speedFactor( 1.f )
        , m_freeze( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Affector::~Affector()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setAffectorType( EAffectorType _type ) noexcept
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    EAffectorType Affector::getAffectorType() const noexcept
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setId( AFFECTOR_ID _id ) noexcept
    {
        m_id = _id;
    }
    //////////////////////////////////////////////////////////////////////////
    AFFECTOR_ID Affector::getId() const noexcept
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setSpeedFactor( float _speedAffector ) noexcept
    {
        m_speedFactor = _speedAffector;
    }
    //////////////////////////////////////////////////////////////////////////
    float Affector::getSpeedFactor() const noexcept
    {
        return m_speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::setFreeze( bool _value ) noexcept
    {
        m_freeze = _value;

        this->_setFreeze( m_freeze );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::getFreeze() const noexcept
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void Affector::_setFreeze( bool _value )
    {
        (void)_value;

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::prepare()
    {
        bool successful = this->_prepare();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::_prepare()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Affector::affect( const UpdateContext * _context, float * _used )
    {
        if( m_freeze == true )
        {
            return false;
        }

        UpdateContext affectContext = *_context;
        affectContext.time *= m_speedFactor;

        bool isEnd = this->_affect( &affectContext, _used );

        return isEnd;
    }
}
