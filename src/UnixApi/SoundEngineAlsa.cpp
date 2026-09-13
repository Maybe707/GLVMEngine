// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "SoundEngineAlsa.hpp"
#include <array>
#include <chrono>
#include <fstream>
#include <memory>
#include <cstring>

namespace GLVM::core::Sound
{
    void CSoundEngineAlsa::OpenDevice(const char* device) {
        CloseDevice();
        // WSLg exposes PulseAudio rather than a physical ALSA card.
        if (std::strcmp(device, "default") == 0 && std::getenv("PULSE_SERVER"))
            device = "pulse";
        const int result = snd_pcm_open(&pPcm, device, SND_PCM_STREAM_PLAYBACK, 0);
        if (result < 0) {
            pPcm = nullptr;
            std::cerr << "Audio unavailable: " << snd_strerror(result) << '\n';
        }
    }

    void CSoundEngineAlsa::CloseDevice() {
        if (!pPcm) return;
        snd_pcm_drop(pPcm);
        snd_pcm_close(pPcm);
        pPcm = nullptr;
    }

    void CSoundEngineAlsa::SoundStream() {
        std::unique_ptr<CSoundSample> sample;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueReady.wait_for(lock, std::chrono::milliseconds(10),
                                [this] { return !tSound_Contaier.empty(); });
            if (tSound_Contaier.empty()) return;
            sample.reset(tSound_Contaier.GetFirstItem());
            tSound_Contaier.RemoveFirstItem();
        }
        PlaybackSoundSample(*sample);
    }

    void CSoundEngineAlsa::PlaybackSoundSample(CSoundSample& sample) {
        if (!pPcm || !sample.kPath_to_File_) return;
        std::ifstream input(sample.kPath_to_File_, std::ios::binary);
        char header[12];
        if (!input.read(header, sizeof(header)) || std::memcmp(header, "RIFF", 4) ||
            std::memcmp(header + 8, "WAVE", 4)) return;

        auto le16 = [](const unsigned char* bytes) -> unsigned int {
            return bytes[0] | (unsigned(bytes[1]) << 8);
        };
        auto le32 = [](const unsigned char* bytes) -> uint32_t {
            return uint32_t(bytes[0]) | (uint32_t(bytes[1]) << 8) |
                   (uint32_t(bytes[2]) << 16) | (uint32_t(bytes[3]) << 24);
        };
        unsigned int channels = 0, rate = 0;
        uint32_t dataSize = 0;
        unsigned char chunk[8];
        while (input.read(reinterpret_cast<char*>(chunk), sizeof(chunk))) {
            const uint32_t size = le32(chunk + 4);
            if (!std::memcmp(chunk, "fmt ", 4)) {
                unsigned char format[16];
                if (size < 16 || !input.read(reinterpret_cast<char*>(format), 16)) return;
                channels = le16(format + 2);
                rate = le32(format + 4);
                if (le16(format) != 1 || le16(format + 14) != 16 ||
                    (channels != 1 && channels != 2) || !rate) return;
                input.seekg(static_cast<std::streamoff>(size - 16) + (size & 1), std::ios::cur);
            } else if (!std::memcmp(chunk, "data", 4)) {
                if (!channels || !rate) return;
                dataSize = size;
                break;
            } else {
                input.seekg(static_cast<std::streamoff>(size) + (size & 1), std::ios::cur);
            }
        }
        if (!dataSize || snd_pcm_set_params(pPcm, SND_PCM_FORMAT_S16_LE,
                SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 1, 50000) < 0) return;
        const unsigned int frameSize = channels * sizeof(int16_t);
        const float volume = std::isfinite(sample.volume) ? std::clamp(sample.volume, 0.0f, 1.0f) : 0.0f;
        std::array<int16_t, 1024> samples{};
        while (dataSize >= frameSize) {
            const auto bytes = std::min<uint32_t>(dataSize, sizeof(samples));
            input.read(reinterpret_cast<char*>(samples.data()), bytes);
            const auto readBytes = static_cast<uint32_t>(input.gcount());
            if (!readBytes) break;
            dataSize -= readBytes;
            const auto frames = readBytes / frameSize;
            for (size_t i = 0; i < frames * channels; ++i)
                samples[i] = static_cast<int16_t>(samples[i] * volume);
            snd_pcm_uframes_t written = 0;
            while (written < frames) {
                const auto result = snd_pcm_writei(pPcm, samples.data() + written * channels, frames - written);
                if (result < 0) {
                    if (snd_pcm_recover(pPcm, static_cast<int>(result), 1) < 0) return;
                } else if (!result) {
                    return;
                } else {
                    written += result;
                }
            }
        }
    }

    void CSoundEngineAlsa::SetMasterVolume(long _lVolume)
    {
        long lMin, lMax;
        snd_mixer_t* pHandle;
        snd_mixer_selem_id_t* pSid;
        const char* pCard = "default";
        const char* pSelem_Name = "Master";

        if (snd_mixer_open(&pHandle, 0) < 0) return;
        if (snd_mixer_attach(pHandle, pCard) < 0 ||
            snd_mixer_selem_register(pHandle, NULL, NULL) < 0 || snd_mixer_load(pHandle) < 0) {
            snd_mixer_close(pHandle);
            return;
        }

        snd_mixer_selem_id_alloca(&pSid);
        snd_mixer_selem_id_set_index(pSid, 0);
        snd_mixer_selem_id_set_name(pSid, pSelem_Name);
        snd_mixer_elem_t* pElem = snd_mixer_find_selem(pHandle, pSid);

        if (!pElem) { snd_mixer_close(pHandle); return; }
        _lVolume = std::clamp(_lVolume, 0L, 100L);
        snd_mixer_selem_get_playback_volume_range(pElem, &lMin, &lMax);
        snd_mixer_selem_set_playback_volume_all( pElem, lMin + (_lVolume * (lMax - lMin)) / 100 );

        snd_mixer_close(pHandle);
    }
        
    vector<CSoundSample*>& CSoundEngineAlsa::GetSoundContainer() { return tSound_Contaier; }

	void CSoundEngineAlsa::CreateSoundSample( const char* filePath, u32 duration, u32 rate, float volume ) {
		core::Sound::CSoundSample* pSound_Sample = new core::Sound::CSoundSample();
		pSound_Sample->kPath_to_File_ = filePath;
		pSound_Sample->uiDuration_ = duration;
		pSound_Sample->uiRate_ = rate;
		pSound_Sample->volume  = volume;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tSound_Contaier.Push(pSound_Sample);
        }
        queueReady.notify_one();
	}
	
	CSoundEngineAlsa::~CSoundEngineAlsa() {
        CloseDevice();
		for( u32 i = 0; i < tSound_Contaier.GetSize(); ++i ) {
			delete tSound_Contaier[i];
			tSound_Contaier[i] = nullptr;
		}
	}
}
