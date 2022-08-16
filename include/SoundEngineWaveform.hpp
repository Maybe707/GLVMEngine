#ifndef SOUND_ENGINE_WAVEFORM
#define SOUND_ENGINE_WAVEFORM

#include "ISoundEngine.hpp"
#include <windows.h>
#include <mmsystem.h>

namespace GLVM::Core::Sound
{
    class CSoundEngineWaveform : public ISoundEngine
    {
        HANDLE hData  = NULL;  ///< handle of waveform data memory 
        HPSTR  lpData = NULL;  ///< pointer to waveform data memory
        
        TCVectorContainer<CSoundSample*> tSound_Contaier;
    public:
        void SoundStream() override;
        void PlaybackSoundSample(CSoundSample& _sound_sample) override;
        void SetMasterVolume(long _lVolume) override;
        TCVectorContainer<CSoundSample*>& GetSoundContainer() override;  
    };
}

#endif
