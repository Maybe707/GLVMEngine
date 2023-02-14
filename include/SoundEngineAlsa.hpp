#ifndef SOUND_ENGINE
#define SOUND_ENGINE

#include "Vector.hpp"
#include "Event.hpp"
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <bits/types/FILE.h>
#include "ISoundEngine.hpp"

namespace GLVM::core::Sound
{
    class CSoundEngineAlsa : public ISoundEngine
    {
        vector<CSoundSample*> tSound_Contaier;
    public:
        void SoundStream() override;
        void PlaybackSoundSample(CSoundSample& _sound_sample) override;
        void SetMasterVolume(long _lVolume) override;
        vector<CSoundSample*>& GetSoundContainer() override;
    };
}

#endif
