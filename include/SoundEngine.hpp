#ifndef SOUND_ENGINE
#define SOUND_ENGINE

#include "VectorContainer.hpp"
#include "Event.hpp"
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <bits/types/FILE.h>

namespace GLVM::Core
{
    struct CSoundSample
    {
        const char* kPath_to_File_;
        unsigned int uiDuration_;
        unsigned int uiRate_;
    };
    
    class CSoundEngine
    {
        TCVectorContainer<CSoundSample*> tSound_Contaier;
    public:
        void SoundStream()
        {
            for(int i = 0; i < tSound_Contaier.GetSize(); ++i)
            {
                std::cout << (*tSound_Contaier[i]).kPath_to_File_ << std::endl;
                std::cout << (*tSound_Contaier[i]).uiDuration_ << std::endl;
                std::cout << (*tSound_Contaier[i]).uiRate_ << std::endl;
                PlaybackSoundSample(*tSound_Contaier[i]);
                tSound_Contaier.RemoveObject(tSound_Contaier[i]);
            }
        }

        void PlaybackSoundSample(CSoundSample& _sound_sample)
        {
            const char *kDevice = "default";
            snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
//            snd_pcm_format_t format = SND_PCM_FORMAT_S24_LE;
            snd_pcm_access_t access = SND_PCM_ACCESS_RW_INTERLEAVED;
            unsigned int uiChannels = 2, uiRate;
            unsigned int uiLatency = 500000; /* 0.5 s */
            snd_pcm_t *pPcm;
            unsigned int uiFrame_Size = uiChannels * 2;

            uiRate = _sound_sample.uiRate_;
            (snd_pcm_open(&pPcm, kDevice, SND_PCM_STREAM_PLAYBACK, 0));
            (snd_pcm_set_params(pPcm, format, access, uiChannels, uiRate, 1, uiLatency));

#define FRAMES 32
            char* buf, *data;
            int frames, rest;
            FILE* iFile_Descritor;

            iFile_Descritor = fopen(_sound_sample.kPath_to_File_, "r");
            
            buf = (char*)malloc(FRAMES * uiFrame_Size);
            for (;;) {
                frames = fread(buf, uiFrame_Size, FRAMES, iFile_Descritor);
                if (frames <= 0)
                    break;
                rest = frames;
                data = buf;
                while (rest > 0) {
                    frames = snd_pcm_writei(pPcm, data, rest);
                    // if (frames < 0)
                    //     CHECK(snd_pcm_recover(pPcm, frames, 0));
                    // else {
                        rest -= frames;
                        data += frames * uiFrame_Size;
//                    }
                }
            }
            free(buf);

            snd_pcm_drain(pPcm);
            snd_pcm_close(pPcm);
        }


        
        TCVectorContainer<CSoundSample*>& GetSoundContaier() { return tSound_Contaier; }
    };
}

#endif
