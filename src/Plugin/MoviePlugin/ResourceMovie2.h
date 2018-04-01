#pragma once

#include "Kernel/ResourceReference.h"

#include "movie/movie.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceMovie2
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceMovie2();
		~ResourceMovie2() override;

	public:
		void setMovieInstance( const aeMovieInstance * _instance );

    public:
        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

    public:
        bool hasComposition( const ConstString & _name ) const;
        float getCompositionDuration( const ConstString & _name ) const;
        float getCompositionFrameDuration( const ConstString & _name ) const;

	public:
		const aeMovieData * getMovieData() const;
		const aeMovieCompositionData * getCompositionData( const ConstString & _name ) const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
        bool _isValid() const override;

    public:        
        ResourceReference * getResource_( const ae_string_t _name );

	public:
		ResourceReference * createResourceImage_( const aeMovieResourceImage * _resource );
		ResourceReference * createResourceVideo_( const aeMovieResourceVideo * _resource );
		ResourceReference * createResourceSound_( const aeMovieResourceSound * _resource );
        ResourceReference * createResourceParticle_( const aeMovieResourceParticle * _resource );

	protected:
        const aeMovieInstance * m_instance;

		aeMovieData * m_movieData;

        FilePath m_filePath;

		typedef stdex::vector<ResourceReferencePtr> TVectorResources;
		TVectorResources m_resources;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceMovie2> ResourceMovie2Ptr;
}