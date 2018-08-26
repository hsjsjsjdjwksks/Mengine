#pragma once

#include "Interface/SoundSystemInterface.h"

#include "OpenALSoundBufferBase.h"

#include "AL/al.h"

namespace Mengine
{
    class OALSoundSystem;
    //////////////////////////////////////////////////////////////////////////
    class OALSoundSource
        : public ServantBase<SoundSourceInterface>
    {
    public:
        OALSoundSource();
        ~OALSoundSource() override;

    public:
        void initialize( OALSoundSystem* _soundSystem );

    public:
        bool play() override;
        void pause() override;
        void stop() override;

    public:
        bool isPlay() const override;
        bool isPause() const override;

        void setVolume( float _volume ) override;
        float getVolume() const override;

        void setLoop( bool _loop ) override;
        bool getLoop() const override;

        float getDuration() const override;
        float getPosition() const override;
        bool setPosition( float _posMs ) override;

        void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) override;
        SoundBufferInterfacePtr getSoundBuffer() const override;

    public:
        void setHeadMode( bool _headMode );
        bool getHeadMode() const;

    protected:
        void unloadBuffer_();
        void releaseSourceId_();

    protected:
        void apply_( ALuint _source );

    protected:
        OALSoundSystem * m_soundSystem;

        mt::vec3f m_position;
        float m_volume;

        ALuint m_sourceId;
        float m_timing;

        OALSoundBufferBasePtr m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_pausing;
        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OALSoundSource> OALSoundSourcePtr;
}
