#pragma once

#include "Interface/TextFontInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Reference.h"

namespace Mengine
{
    class FontBase
        : public TextFontInterface
        , public Compilable
        , public Reference
    {
    public:
        FontBase();
        ~FontBase() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        void setType( const ConstString & _name ) override;
        const ConstString & getType() const override;

    public:
        bool compileFont() override;
        void releaseFont() override;

    public:
        bool _incrementZero() override;
        void _decrementZero() override;

    public:
        bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) override;
        bool unfetch() override;

    protected:
        virtual bool _prefetch( const PrefetcherObserverInterfacePtr & _observer );
        virtual bool _unfetch();

    public:
        void setHeight( uint32_t _height ) override;
        uint32_t getHeight() const override;

        void setFontColor( const Color & _color ) override;
        const Color & getFontColor() const override;

        void setLineOffset( float _lineOffset ) override;
        float getLineOffset() const override;

        void setCharOffset( float _charOffset ) override;
        float getCharOffset() const override;

    public:
        uint32_t getFontParams() const override;

    public:
        bool validateText( const ConstString & _key, const Char * _text, size_t _size ) const override;
        U32String prepareText( const Char * _text, size_t _size ) override;
        bool prepareGlyph( const U32String & _text, const DocumentPtr & _doc ) override;

    protected:
        virtual bool _validateGlyphes( const U32String & _codes ) const = 0;
        virtual bool _prepareGlyph( GlyphCode _code, const DocumentPtr & _doc ) = 0;

    protected:
        ConstString m_name;
        ConstString m_type;

        uint32_t m_params;

        uint32_t m_height;

        Color m_colorFont;

        float m_lineOffset;
        float m_charOffset;
    };
}
