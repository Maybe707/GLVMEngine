#include "SoundEngineWaveform.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>

namespace GLVM::core::Sound {
void CSoundEngineWaveform::CloseDevice() {
    RequestStop();
    std::lock_guard lock(mutex_);
    queue_.clear();
}
void CSoundEngineWaveform::CreateSoundSample(const char* path, u32 duration, u32 rate, float volume) {
    if (!path || stopped_) return;
    {
        std::lock_guard lock(mutex_);
        queue_.push_back({path, duration, rate, volume});
    }
    ready_.notify_one();
}
void CSoundEngineWaveform::SetMasterVolume(long volume) {
    volume_ = std::clamp(volume, 0L, 100L) / 100.0f;
}
void CSoundEngineWaveform::SoundStream() {
    Request request;
    {
        std::unique_lock lock(mutex_);
        ready_.wait_for(lock, std::chrono::milliseconds(10), [this] { return stopped_ || !queue_.empty(); });
        if (stopped_ || queue_.empty()) return;
        request = std::move(queue_.front());
        queue_.pop_front();
    }
    CSoundSample sample{request.path.c_str(), request.duration, request.rate, request.volume};
    PlaybackSoundSample(sample);
}
void CSoundEngineWaveform::PlaybackSoundSample(CSoundSample& sample) {
    if (stopped_ || !sample.kPath_to_File_) return;
    std::ifstream input(sample.kPath_to_File_, std::ios::binary);
    char header[12];
    if (!input.read(header, 12) || std::memcmp(header, "RIFF", 4) || std::memcmp(header + 8, "WAVE", 4)) return;
    auto le16 = [](const unsigned char* bytes) -> WORD { return bytes[0] | (WORD(bytes[1]) << 8); };
    auto le32 = [](const unsigned char* bytes) -> DWORD {
        return DWORD(bytes[0]) | DWORD(bytes[1]) << 8 | DWORD(bytes[2]) << 16 | DWORD(bytes[3]) << 24;
    };
    WAVEFORMATEX format{};
    std::vector<int16_t> pcm;
    unsigned char chunk[8];
    while (input.read(reinterpret_cast<char*>(chunk), 8)) {
        const DWORD size = le32(chunk + 4);
        if (!std::memcmp(chunk, "fmt ", 4)) {
            unsigned char data[16];
            if (size < 16 || !input.read(reinterpret_cast<char*>(data), 16)) return;
            format.wFormatTag = le16(data);
            format.nChannels = le16(data + 2);
            format.nSamplesPerSec = le32(data + 4);
            format.nAvgBytesPerSec = le32(data + 8);
            format.nBlockAlign = le16(data + 12);
            format.wBitsPerSample = le16(data + 14);
            if (format.wFormatTag != WAVE_FORMAT_PCM || format.wBitsPerSample != 16 ||
                (format.nChannels != 1 && format.nChannels != 2) || !format.nSamplesPerSec ||
                format.nBlockAlign != format.nChannels * sizeof(int16_t) ||
                format.nAvgBytesPerSec != format.nSamplesPerSec * format.nBlockAlign) return;
            input.seekg(static_cast<std::streamoff>(size - 16) + (size & 1), std::ios::cur);
        } else if (!std::memcmp(chunk, "data", 4)) {
            if (!format.nBlockAlign || !size || size > 32 * 1024 * 1024 || size % format.nBlockAlign) return;
            pcm.resize(size / sizeof(int16_t));
            if (!input.read(reinterpret_cast<char*>(pcm.data()), size)) return;
            break;
        } else input.seekg(static_cast<std::streamoff>(size) + (size & 1), std::ios::cur);
    }
    if (pcm.empty()) return;
    const float gain = std::isfinite(sample.volume) ? std::clamp(sample.volume, 0.0f, 1.0f) * volume_.load() : 0.0f;
    for (auto& value : pcm) value = static_cast<int16_t>(value * gain);
    HANDLE completed = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!completed) return;
    HWAVEOUT output{};
    if (waveOutOpen(&output, WAVE_MAPPER, &format, reinterpret_cast<DWORD_PTR>(completed), 0, CALLBACK_EVENT) != MMSYSERR_NOERROR) {
        CloseHandle(completed);
        return;
    }
    WAVEHDR buffer{};
    buffer.lpData = reinterpret_cast<char*>(pcm.data());
    buffer.dwBufferLength = static_cast<DWORD>(pcm.size() * sizeof(int16_t));
    if (waveOutPrepareHeader(output, &buffer, sizeof(buffer)) == MMSYSERR_NOERROR) {
        if (waveOutWrite(output, &buffer, sizeof(buffer)) == MMSYSERR_NOERROR) {
            while (!(buffer.dwFlags & WHDR_DONE) && !stopped_) WaitForSingleObject(completed, 10);
        }
        waveOutReset(output);
        waveOutUnprepareHeader(output, &buffer, sizeof(buffer));
    }
    waveOutClose(output);
    CloseHandle(completed);
}
}
