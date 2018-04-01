#pragma once

#include "Interface/FileSystemInterface.h"
#include "Interface/WindowsLayerInterface.h"

#include "Core/ServantBase.h"

#include "stdex/thread_guard.h"

namespace Mengine
{

#ifndef MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE
#	define MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE MENGINE_FILE_STREAM_BUFFER_SIZE
#endif

	class Win32FileInputStream
		: public ServantBase<FileInputStreamInterface>
	{
	public:
		Win32FileInputStream();
		~Win32FileInputStream() override;

	public:
		bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size ) override;
		
	public:
		size_t read( void * _buf, size_t _count ) override;
		bool seek( size_t _pos ) override;
		bool skip( size_t _pos ) override;
		size_t tell() const override;
		size_t size() const override;
		bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

	public:
		bool memory( void ** _memory, size_t * _size ) override;

    protected:
		bool openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * _fullPath );
        void close_();
		bool read_( void * _buf, size_t _size, size_t & _read );
		bool seek_( size_t _pos );

    protected:
		HANDLE m_hFile;

		size_t m_size;
		size_t m_offset;

		size_t m_carriage;
		size_t m_capacity;
		size_t m_reading;

		uint8_t m_readCache[MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE];

		STDEX_THREAD_GUARD_INIT;

#ifdef _DEBUG
        String m_relationPath;
		String m_folder;
		String m_fileName;
#endif
	};
}	
