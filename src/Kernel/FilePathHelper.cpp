#include "FilePathHelper.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const Char * _value )
        {
            ConstString cstr = Helper::stringizeString( _value );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathSize( const Char * _value, FilePath::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringSize( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const String & _filePath )
        {
            const Char * value_str = _filePath.c_str();
            String::size_type value_size = _filePath.size();

            FilePath fp = Helper::stringizeFilePathSize( value_str, (FilePath::size_type)value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePath( const PathString & _filePath )
        {
            const Char * value_str = _filePath.c_str();
            PathString::size_type value_size = _filePath.size();

            FilePath fp = Helper::stringizeFilePathSize( value_str, value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathLocal( const Char * _value, FilePath::size_type _size )
        {
            ConstString cstr = Helper::stringizeStringLocal( _value, _size );

            return FilePath( cstr );
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathLocal( const PathString & _filePath )
        {
            const Char * value_str = _filePath.c_str();
            PathString::size_type value_size = _filePath.size();

            FilePath fp = Helper::stringizeFilePathLocal( value_str, value_size );

            return fp;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFilePathFormat( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Char str[MENGINE_MAX_PATH] = { '\0' };
            int32_t size = MENGINE_VSNPRINTF( str, MENGINE_MAX_PATH - 1, _format, args );

            MENGINE_ASSERTION_FATAL( size >= 0, "invalid stringize file format '%s'"
                , _format
            );

            MENGINE_VA_LIST_END( args );

            if( size == 0 )
            {
                return FilePath::none();
            }

            FilePath filePath = Helper::stringizeFilePathSize( str, size );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        FilePath stringizeFileHashUnique( const Char * _value, FilePath::size_type _size, FilePath::hash_type _hash )
        {
            ConstString constString = Helper::stringizeStringHashUnique( _value, _size, _hash );

            return FilePath( constString );
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString getFilePathExt( const FilePath & _filePath )
        {
            const Char * str_filePath = _filePath.c_str();

            const Char * dot_find = MENGINE_STRRCHR( str_filePath, '.' );

            if( dot_find == nullptr )
            {
                return ConstString::none();
            }

            ConstString ext = Helper::stringizeString( dot_find + 1 );

            return ext;
        }
    }
}