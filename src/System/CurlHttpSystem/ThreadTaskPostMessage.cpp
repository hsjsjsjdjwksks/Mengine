#	include "ThreadTaskPostMessage.h"

#	include "Interface/ConfigInterface.h"

#	include "Logger/Logger.h"

#   include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    ThreadTaskPostMessage::ThreadTaskPostMessage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskPostMessage::initialize( const String & _url, const TMapParams & _params )
	{
		m_url = _url;
		m_params = _params;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskPostMessage::_onCURL( CURL * _curl )
	{		
		/* specify URL to get */
		curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

		curl_easy_setopt( _curl, CURLOPT_POST, 1 );

		struct curl_httppost * lastptr = nullptr;
		curl_httppost * formpost = nullptr;

		for( TMapParams::const_iterator
			it = m_params.begin(),
			it_end = m_params.end();
			it != it_end;
			++it )
		{
			const ConstString & key = it->first;
			const String & value = it->second;

			/* Fill in the  field */
			curl_formadd( &formpost,
				&lastptr,
				CURLFORM_COPYNAME, key.c_str(),
				CURLFORM_COPYCONTENTS, value.c_str(),
				CURLFORM_END );
		}

		curl_easy_setopt( _curl, CURLOPT_HTTPPOST, formpost );
        
		/* send all data to this function  */
        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( m_serviceProvider, "HTTP", "Log", false ) == true )
        {
            std::stringstream ss;

            for( TMapParams::const_iterator
                it = m_params.begin(),
                it_end = m_params.end();
                it != it_end;
                ++it )
            {
                ss << it->first.c_str() << ": " << it->second;
                ss << std::endl;
            }

            std::string params_str = ss.str();

            LOGGER_STATISTIC( m_serviceProvider )("HTTP: post message url '%s' params:\n %s"
                , m_url.c_str()
                , params_str.c_str()
                );
        }
	}
}
