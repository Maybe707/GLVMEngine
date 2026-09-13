#include "SoundEngineWaveform.hpp"
#include "Runtime/AudioService.hpp"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

int main() {
    using namespace GLVM::core::Sound;
    const auto path = std::filesystem::temp_directory_path() / "glvm-windows-audio-test.wav";
    // One second of silent mono PCM. RequestStop must cancel it promptly.
    {
        std::ofstream wav(path, std::ios::binary);
        const uint32_t rate = 8000, size = rate * 2, riffSize = size + 36;
        const uint16_t one = 1, bits = 16, align = 2;
        const uint32_t fmtSize = 16, bytesPerSecond = rate * align;
        auto put = [&wav](const auto& value) { wav.write(reinterpret_cast<const char*>(&value), sizeof(value)); };
        wav.write("RIFF", 4); put(riffSize); wav.write("WAVEfmt ", 8); put(fmtSize);
        put(one); put(one); put(rate); put(bytesPerSecond); put(align); put(bits);
        wav.write("data", 4); put(size);
        for (uint32_t i = 0; i < rate; ++i) { int16_t silence = 0; put(silence); }
    }
    {
        AudioService audio(std::make_unique<CSoundEngineWaveform>());
        audio.start();
        const std::string file = path.string();
        audio.backend().CreateSoundSample("missing.wav", 0, 8000, 1);
        audio.backend().CreateSoundSample(file.c_str(), 0, 8000, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        audio.rethrowWorkerError();
        const auto stop = std::chrono::steady_clock::now();
        audio.stop();
        assert(std::chrono::steady_clock::now() - stop < std::chrono::milliseconds(500));
        audio.stop();
        audio.start();
        audio.stop();
    }
    std::filesystem::remove(path);
    std::cout << "PASS Windows audio queue, WAV playback, cancellation and restart\n";
}
