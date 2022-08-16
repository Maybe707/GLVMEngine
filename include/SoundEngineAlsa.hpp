#ifndef SOUND_ENGINE
#define SOUND_ENGINE

#include "VectorContainer.hpp"
#include "Event.hpp"
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <bits/types/FILE.h>
#include "ISoundEngine.hpp"

namespace GLVM::Core::Sound
{
    class CSoundEngineAlsa : public ISoundEngine
    {
        TCVectorContainer<CSoundSample*> tSound_Contaier;
    public:
        void SoundStream() override;
        void PlaybackSoundSample(CSoundSample& _sound_sample);
        void SetMasterVolume(long _lVolume) override;
        TCVectorContainer<CSoundSample*>& GetSoundContainer() override;
    };
}

#endif
