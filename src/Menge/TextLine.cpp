#	include "TextLine.h"

#	include "ResourceFont.h"
#	include "ResourceGlyph.h"

#	include "Glyph.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#   include <utf8.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( ServiceProviderInterface * _serviceProvider, float _height, float _charOffset )
		: m_serviceProvider(_serviceProvider)
        , m_charOffset(_charOffset)
		, m_height(_height)
		, m_length(0)
		, m_invalidateTextLine(true)
		, m_offset(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::initialize( const ResourceFont * _resource, const String& _text )
	{
		WString::size_type text_size = _text.length();
		m_charsData.reserve( text_size );

		float totalKerning = 0.0f;
		
        const char * text_str = _text.c_str();
        size_t text_len = _text.size();

        //const char * text_it = text_str;
        //const char * text_end = text_str + text_len + 1;

		const RenderTextureInterfacePtr & image = _resource->getTextureFont();

		float inv_image_width = 1.f / (float)image->getWidth();
		float inv_image_height = 1.f / (float)image->getHeight();

		const ResourceGlyph * resourceGlyph = _resource->getResourceGlyph();
		float initSize = 1.f / resourceGlyph->getFontHeight();

		for( const char
            *text_it = text_str,
            *text_end = text_str + text_len + 1;
        text_it != text_end;
        )
		{			
			size_t code = 0;
			utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );
			
			if( err != utf8::internal::UTF8_OK )
			{
				LOGGER_ERROR(m_serviceProvider)( "TextLine for resource %s invalid glyph |%s| err code %d"
					, _resource->getName().c_str()
					, text_it
					, err
					);

				continue;
			}

            if( code == 0 )
            {
                continue;
            }

            GlyphChar glyphChar;
            glyphChar.setCode( code );

            const Glyph * glyph;
			if( _resource->hasGlyph( glyphChar, &glyph ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "TextLine for resource %s invalid glyph %d"
					, _resource->getName().c_str()
					, code
					);

				continue;
			}

			
					
			CharData charData;

			charData.code = glyphChar;	

			mt::ident_v3( charData.renderVertex[0] );
			mt::ident_v3( charData.renderVertex[1] );
			mt::ident_v3( charData.renderVertex[2] );
			mt::ident_v3( charData.renderVertex[3] );

			charData.uv = glyph->getUV();

			charData.uv.x *= inv_image_width;
			charData.uv.y *= inv_image_height;
			charData.uv.z *= inv_image_width;
			charData.uv.w *= inv_image_height;

			charData.ratio = glyph->getRatio();
			charData.offset = glyph->getOffset();
			
			charData.size = glyph->getSize();
			
			charData.size *= m_height;

			

			
			charData.size *= initSize;
			
			if( m_charsData.empty() == false )
			{
				const CharData & prevChar = m_charsData.back();

                GlyphChar prev_code = prevChar.code;

				const Glyph * prevGlyph; 				
				if( _resource->hasGlyph( prev_code, &prevGlyph ) == true )
				{
	                GlyphChar curr_code = charData.code;
					float kerning = prevGlyph->getKerning( curr_code );

					totalKerning += kerning;
				}
			}

			charData.offset.x += totalKerning;
			m_charsData.push_back( charData );

			float height = m_height;
			float width = floorf( charData.ratio * height );
			m_length += width + m_charOffset;
		}

		m_length -= m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	int TextLine::getCharsDataSize() const
	{
		return m_charsData.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::prepareRenderObject(	mt::vec2f & _offset		
		, const mt::vec4f & _uv
		, unsigned int _argb	
        , bool _pixelsnap
		, TVectorRenderVertex2D& _renderObject ) const
	{
		if( m_invalidateTextLine == true )
		{
			this->updateRenderLine_( _offset );
		}

		//_renderObject->vertices.clear();
		//_renderObject->passes[0].indicies.clear();
		//_renderObject->material.color = ;

		size_t renderObjectNum = _renderObject.size();

		_renderObject.resize( renderObjectNum + m_charsData.size() * 4 );

		for( TVectorCharData::const_iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
            const CharData & data = *it_char;

			for( int i = 0; i != 4; ++i )
			{
				//_renderObject->vertices.push_back( TVertex() );
				RenderVertex2D & renderVertex = _renderObject[renderObjectNum + i];
				const mt::vec3f & charVertex = data.renderVertex[i];
				

                if( _pixelsnap == true )
                {
                    renderVertex.pos[0] = floorf( charVertex.x + 0.5f );
                    renderVertex.pos[1] = floorf( charVertex.y + 0.5f );
                }
                else
                {
                    renderVertex.pos[0] = charVertex.x;
                    renderVertex.pos[1] = charVertex.y;
                }                

				renderVertex.pos[2] = charVertex.z;

				renderVertex.color = _argb;

                renderVertex.uv[0] = 0.f;
                renderVertex.uv[1] = 0.f;

                renderVertex.uv2[0] = 0.f;
                renderVertex.uv2[1] = 0.f;
            }

			const mt::vec4f & char_uv = data.uv;

			mt::vec4f total_uv;
			total_uv.x = _uv.x + (_uv.z - _uv.x) * char_uv.x;
			total_uv.y = _uv.y + (_uv.w - _uv.y) * char_uv.y;
			total_uv.z = _uv.x + (_uv.z - _uv.x) * char_uv.z;
			total_uv.w = _uv.y + (_uv.w - _uv.y) * char_uv.w;

			_renderObject[renderObjectNum + 0].uv[0] = total_uv.x;
			_renderObject[renderObjectNum + 0].uv[1] = total_uv.y;
			_renderObject[renderObjectNum + 1].uv[0] = total_uv.z;
			_renderObject[renderObjectNum + 1].uv[1] = total_uv.y;
			_renderObject[renderObjectNum + 2].uv[0] = total_uv.z;
			_renderObject[renderObjectNum + 2].uv[1] = total_uv.w;
			_renderObject[renderObjectNum + 3].uv[0] = total_uv.x;
			_renderObject[renderObjectNum + 3].uv[1] = total_uv.w;

			renderObjectNum += 4;
		}

		_offset.x += m_offset;

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateRenderLine_( mt::vec2f& _offset ) const
	{
		for( TVectorCharData::iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			CharData & cd = *it_char;

			float width = cd.ratio * m_height;

			mt::vec2f size = cd.size;

			mt::vec2f offset = _offset + cd.offset;
			mt::vec3f v3_offset(offset.x, offset.y, 0.f);
			
            cd.renderVertex[0] = v3_offset;
			cd.renderVertex[1] = v3_offset + mt::vec3f(size.x, 0.0f, 0.f);
			cd.renderVertex[2] = v3_offset + mt::vec3f(size.x, size.y, 0.f);
			cd.renderVertex[3] = v3_offset + mt::vec3f(0.0f, size.y, 0.f);

			_offset.x += width + m_charOffset;
		}

		m_offset = _offset.x;

		m_invalidateTextLine = false;
	}
	//////////////////////////////////////////////////////////////////////////
}
