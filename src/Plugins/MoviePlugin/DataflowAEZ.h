#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

#include "movie/movie.hpp"

namespace Mengine
{
    class DataflowAEZ
        : public DataflowInterface
        , public Factorable
    {
    public:
        DataflowAEZ();
        ~DataflowAEZ() override;

    public:
        void setMovieInstance( const aeMovieInstance * _movieInstance );
        const aeMovieInstance * getMovieInstance() const;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        DataInterfacePtr create() override;

    public:
        bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream, const Char * _doc ) override;

    protected:
        const aeMovieInstance * m_movieInstance;

        ArchivatorInterfacePtr m_archivator;

        FactoryPtr m_factoryMovieData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowAEZ> DataflowAEZPtr;
    //////////////////////////////////////////////////////////////////////////
}