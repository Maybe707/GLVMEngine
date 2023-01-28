#ifndef ISOUND_ENGINE
#define ISOUND_ENGINE

#include "VectorContainer.hpp"

namespace GLVM::core::Sound
{
    struct CSoundSample
    {
        const char* kPath_to_File_;
        unsigned int uiDuration_;
        unsigned int uiRate_;
    };
    
    class ISoundEngine
    {
    public:
        virtual ~ISoundEngine() {}

        virtual TCVectorContainer<CSoundSample*>& GetSoundContainer() = 0;
        virtual void PlaybackSoundSample(CSoundSample& _sound_sample) = 0;
        virtual void SetMasterVolume(long _lVolume) = 0;
        virtual void SoundStream() = 0;
    };
}

#endif
