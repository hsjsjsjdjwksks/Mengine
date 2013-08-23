#	include "TextManager.h"

#	include "Core/String.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#   include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TextService, Menge::TextServiceInterface, Menge::TextManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextManager::TextManager()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TextManager::~TextManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void TextManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * TextManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void TextManager::initialize( size_t _size )
	{
		m_textMap.reserve(_size);
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::addTextEntry( const ConstString& _key, const TextEntry & _entry )
	{
		TextEntry * textEntry = nullptr;
		if( m_textMap.has( _key, &textEntry ) == true )
		{
			LOGGER_INFO(m_serviceProvider)( "TextManager::addTextEntry: duplicate key found %s"
				, _key.c_str()
				);

			*textEntry = _entry;

            return;
		}

        m_textMap.insert( _key, _entry );		
	}
	//////////////////////////////////////////////////////////////////////////
	const TextEntry & TextManager::getTextEntry( const ConstString& _key ) const
	{
		const TextEntry * textEntry = nullptr;
		if( m_textMap.has( _key, &textEntry ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "TextManager::getTextEntry: TextManager can't find string associated with key - '%s'"
				, _key.c_str() 
				);

			static TextEntry emptyEntry;
			emptyEntry.charOffset = 0.f;
			emptyEntry.lineOffset = 0.f;

			return emptyEntry;
		}

        const TextEntry & entry = *textEntry;
        
        return entry;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextManager::existText( const ConstString& _key, const TextEntry ** _entry ) const
	{
		bool result = m_textMap.has( _key, _entry );

		return result;		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextManager::setDefaultResourceFontName( const ConstString & _fontName )
	{
		m_defaultResourceFontName = _fontName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & TextManager::getDefaultResourceFontName() const
	{
		return m_defaultResourceFontName;
	}
}
