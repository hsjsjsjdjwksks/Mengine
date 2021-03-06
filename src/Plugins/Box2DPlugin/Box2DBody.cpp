#include "Box2DBody.h"
#include "Box2DScaler.h"

#include "Kernel/EventableHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DBody::Box2DBody()
        : m_world( nullptr )
        , m_body( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBody::~Box2DBody()
    {
        if( m_world != nullptr && m_body != nullptr )
        {
            m_world->DestroyBody( m_body );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::initialize( const Box2DScaler & _scaler, b2World * _world, const b2BodyDef * _bodyDef )
    {
        m_scaler = _scaler;

        b2Body * b2_body = _world->CreateBody( _bodyDef );

        MENGINE_ASSERTION_MEMORY_PANIC( b2_body );

        b2_body->SetUserData( this );

        m_world = _world;
        m_body = b2_body;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setEventable( const EventablePtr & _eventable )
    {
        m_eventable = _eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    const EventablePtr & Box2DBody::getEventable() const
    {
        return m_eventable;
    }
    //////////////////////////////////////////////////////////////////////////
    b2World * Box2DBody::getWorld() const
    {
        return m_world;
    }
    //////////////////////////////////////////////////////////////////////////
    b2Body * Box2DBody::getBody() const
    {
        return m_body;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeConvex( const Polygon & _polygon
        , float _density
        , float _friction
        , float _restitution
        , bool _isSensor
        , unsigned short _collisionMask
        , unsigned short _categoryBits
        , unsigned short _groupIndex )
    {
        uint32_t num_points = _polygon.size();

        if( num_points > b2_maxPolygonVertices )
        {
            return false;
        }

        b2Vec2 vertices[b2_maxPolygonVertices];
        for( uint32_t i = 0; i != num_points; ++i )
        {
            const mt::vec2f & v = _polygon[i];

            vertices[i] = m_scaler.toBox2DWorld( v );
        }

        b2PolygonShape shape;
        shape.Set( vertices, (int32)num_points );

        b2FixtureDef fd;

        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.maskBits = _collisionMask;
        fd.filter.groupIndex = _groupIndex;
        fd.restitution = _restitution;
        fd.isSensor = _isSensor;

        //density as second argument?
        b2Fixture * fixture = m_body->CreateFixture( &fd );

        if( fixture == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeCircle( float _radius, const mt::vec2f & _position,
        float _density, float _friction, float _restitution, bool _isSensor,
        unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
    {
        b2CircleShape shape;
        shape.m_radius = m_scaler.toBox2DWorld( _radius );
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );
        shape.m_p = b2_position;

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.restitution = _restitution;
        fd.filter.maskBits = _collisionMask;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.groupIndex = _groupIndex;
        fd.isSensor = _isSensor;

        b2Fixture * fixture = m_body->CreateFixture( &fd );

        if( fixture == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeBox( float _width, float _height, const mt::vec2f & _position, float _angle,
        float _density, float _friction, float _restitution, bool _isSensor,
        unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
    {
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );
        float b2_width = m_scaler.toBox2DWorld( _width );
        float b2_height = m_scaler.toBox2DWorld( _height );

        b2PolygonShape shape;
        shape.SetAsBox( b2_width, b2_height
            , b2_position
            , _angle
        );

        b2FixtureDef fd;

        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.restitution = _restitution;
        fd.filter.maskBits = _collisionMask;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.groupIndex = _groupIndex;
        fd.isSensor = _isSensor;

        b2Fixture * fixture = m_body->CreateFixture( &fd );

        if( fixture == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getPosition() const
    {
        const b2Vec2 & b2_position = m_body->GetPosition();

        mt::vec2f position = m_scaler.toEngineWorld( b2_position );

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getAngle() const
    {
        float angle = m_body->GetAngle();

        return angle;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getWorldVector( const mt::vec2f & _localVector )
    {
        b2Vec2 b2_localVector = m_scaler.toBox2DWorld( _localVector );
        b2Vec2 b2_worldVector = m_body->GetWorldVector( b2_localVector );
        mt::vec2f worldVector = m_scaler.toEngineWorld( b2_worldVector );
        return worldVector;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getMass() const
    {
        float mass = m_body->GetMass();

        return mass;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getInertia() const
    {
        float inertia = m_body->GetInertia();

        return inertia;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setLinearVelocity( const mt::vec2f & _velocity )
    {
        b2Vec2 b2_velocity = m_scaler.toBox2DWorld( _velocity );

        m_body->SetLinearVelocity( b2_velocity );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getLinearVelocity() const
    {
        const b2Vec2 & b2_velocity = m_body->GetLinearVelocity();

        mt::vec2f velocity = m_scaler.toEngineWorld( b2_velocity );

        return velocity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setAngularVelocity( float _w )
    {
        m_body->SetAngularVelocity( _w );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getAngularVelocity() const
    {
        return m_body->GetAngularVelocity();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyForce( const mt::vec2f & _force, const mt::vec2f & _point )
    {
        b2Vec2 b2_force = m_scaler.toBox2DWorld( _force );
        b2Vec2 b2_point = m_scaler.toBox2DWorld( _point );

        b2Vec2 b2_world_center = m_body->GetWorldCenter();

        b2Vec2 b2_total_point = b2_world_center + b2_point;
        m_body->ApplyForce( b2_force, b2_total_point, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point )
    {
        b2Vec2 b2_impulse = m_scaler.toBox2DWorld( _impulse );
        b2Vec2 b2_point = m_scaler.toBox2DWorld( _point );

        b2Vec2 b2_world_center = m_body->GetWorldCenter();
        b2Vec2 b2_total_point = b2_world_center + b2_point;
        m_body->ApplyLinearImpulse( b2_impulse, b2_total_point, true );
    }
    //////////////////////////////////////////////////////////////////////////      
    void Box2DBody::applyAngularImpulse( float _impulse )
    {
        m_body->ApplyAngularImpulse( _impulse, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::onBeginContact( Box2DBody * _body, b2Contact * _contact )
    {
        MENGINE_UNUSED( _contact );

        EVENTABLE_OTHER_METHOD( m_eventable, EVENT_BOX2DBODY_BEGIN_CONTACT )
            ->onBox2DBodyBeginContact( _body, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::onEndContact( Box2DBody * _body, b2Contact * _contact )
    {
        MENGINE_UNUSED( _contact );

        EVENTABLE_OTHER_METHOD( m_eventable, EVENT_BOX2DBODY_END_CONTACT )
            ->onBox2DBodyEndContact( _body, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::onPreSolve( Box2DBody * _body, b2Contact * _contact, const b2Manifold * _oldManifold )
    {
        MENGINE_UNUSED( _contact );

        EBox2DManifoldType oldManifoldType = EVENT_BOX2DMANIFOLD_NONE;

        Box2DManifold oldManifold;

        if( _oldManifold->pointCount > 0 )
        {
            oldManifold.localPoint = m_scaler.toEngineWorld( _oldManifold->localPoint );
            oldManifold.localNormal = {_oldManifold->localNormal.x, _oldManifold->localNormal.y};            

            switch( _oldManifold->type )
            {
            case b2Manifold::e_circles:
                {
                    oldManifoldType = EVENT_BOX2DMANIFOLD_CIRCLES;

                    for( int32 index = 0; index != _oldManifold->pointCount; ++index )
                    {
                        const b2ManifoldPoint * b2_point = _oldManifold->points + index;

                        Box2DManifoldPoint point;
                        point.localPoint = m_scaler.toEngineWorld( b2_point->localPoint );
                        point.normalImpulse = b2_point->normalImpulse;
                        point.tangentImpulse = b2_point->tangentImpulse;

                        oldManifold.points[index] = point;
                    }
                }break;
            case b2Manifold::e_faceA:
                {
                    oldManifoldType = EVENT_BOX2DMANIFOLD_FACE_A;
                }break;
            case b2Manifold::e_faceB:
                {
                    oldManifoldType = EVENT_BOX2DMANIFOLD_FACE_B;
                }break;
            default:
                return;
            }
        }

        EVENTABLE_OTHER_METHOD( m_eventable, EVENT_BOX2DBODY_PRE_SOLVE )
            ->onBox2DBodyPreSolve( _body, nullptr, oldManifoldType, (uint32_t)_oldManifold->pointCount, oldManifold );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::onPostSolve( Box2DBody * _body, b2Contact * _contact, const b2ContactImpulse * _impulse )
    {
        MENGINE_UNUSED( _contact );

        Box2DContactImpulse impulse;

        for( int32 index = 0; index != _impulse->count; ++index )
        {
            impulse.normalImpulses[index] = _impulse->normalImpulses[index];
            impulse.tangentImpulses[index] = _impulse->tangentImpulses[index];
        }

        EVENTABLE_OTHER_METHOD( m_eventable, EVENT_BOX2DBODY_POST_SOLVE )
            ->onBox2DBodyPostSolve( _body, nullptr, (uint32_t)_impulse->count, impulse );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setLinearDumping( float _dumping )
    {
        m_body->SetLinearDamping( _dumping );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getLinearDumping() const
    {
        return m_body->GetLinearDamping();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setAngularDumping( float _dumping )
    {
        m_body->SetAngularDamping( _dumping );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getAngularDumping() const
    {
        return m_body->GetAngularDamping();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setFixedRotation( bool _rotation )
    {
        m_body->SetFixedRotation( _rotation );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::getFixedRotation() const
    {
        return m_body->IsFixedRotation();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setIsBullet( bool _isBullet )
    {
        m_body->SetBullet( _isBullet );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::getIsBullet() const
    {
        return m_body->IsBullet();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isFrozen() const
    {
        return !(m_body->IsEnabled());
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isSleeping() const
    {
        return m_body->IsSleepingAllowed();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isStatic() const
    {
        b2BodyType type = m_body->GetType();

        return type == b2_staticBody;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setTransform( const mt::vec2f & _position, float _angle )
    {
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );

        m_body->SetTransform( b2_position, _angle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::sleep()
    {
        m_body->SetSleepingAllowed( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::wakeUp()
    {
        m_body->SetAwake( true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyTorque( float _torque )
    {
        m_body->ApplyTorque( _torque, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex )
    {
        b2Filter filter;
        filter.categoryBits = _categoryBits;
        filter.maskBits = _collisionMask;
        filter.groupIndex = _groupIndex;

        b2Fixture * fixture = m_body->GetFixtureList();
        while( fixture != 0 )
        {
            fixture->SetFilterData( filter );
            //may be not need : m_world->Refilter( shape );
            fixture = fixture->GetNext();
        }
    }
}
