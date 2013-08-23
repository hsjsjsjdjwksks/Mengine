#	include "ParticleConverter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ParticleConverter::ParticleConverter( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleConverter::~ParticleConverter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleConverter::setOptions( ConverterOptions * _options )
	{
		m_options = *(_options);
	}
	//////////////////////////////////////////////////////////////////////////
	void ParticleConverter::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
}
