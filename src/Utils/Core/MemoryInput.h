#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "stdex/thread_guard.h"

namespace Menge
{
	class MemoryInput
		: public MemoryInputInterface
	{
	public:
		MemoryInput();
		~MemoryInput();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        void * newMemory( size_t _size ) override;
		void * copyMemory( const void * _source, size_t _size ) override;

		void trimMemory( size_t _size ) override;

	public:
		const void * getMemory( size_t & _size ) const override;

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
		ServiceProviderInterface * m_serviceProvider;

		unsigned char * m_data;
		size_t m_size;

		unsigned char * m_pos;
		unsigned char * m_end;

		STDEX_THREAD_GUARD_INIT;
	};
}	// namespace Menge


