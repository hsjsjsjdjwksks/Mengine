#	include "ModuleBase.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleBase::ModuleBase()
		: m_serviceProvider( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleBase::~ModuleBase()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ModuleBase::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModuleBase::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::initialize()
	{
		bool successful = this->_initialize();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::finalize()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleBase::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::update( float _time, float _timing )
	{
		this->_update( _time, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		this->_render( _state, _debugMask );
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_update( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleBase::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_state;
		(void)_debugMask;

		//Empty
	}
}