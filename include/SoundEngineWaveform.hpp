#pragma once
#include "ISoundEngine.hpp"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <windows.h>
#include <mmsystem.h>

namespace GLVM::core::Sound {
class CSoundEngineWaveform final : public ISoundEngine {
public:
    void OpenDevice(const char*) override { stopped_ = false; }
    void CloseDevice() override;
    void RequestStop() noexcept override { stopped_ = true; ready_.notify_all(); }
    void SoundStream() override;
    void PlaybackSoundSample(CSoundSample& sample) override;
    void CreateSoundSample(const char* path, u32 duration, u32 rate, float volume) override;
    void SetMasterVolume(long volume) override;
    vector<CSoundSample*>& GetSoundContainer() override { return legacy_; }
private:
    struct Request { std::string path; u32 duration, rate; float volume; };
    std::mutex mutex_;
    std::condition_variable ready_;
    std::deque<Request> queue_;
    std::atomic<bool> stopped_{true};
    std::atomic<float> volume_{1.0f};
    vector<CSoundSample*> legacy_;
};
}
