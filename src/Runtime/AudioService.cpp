#include "Runtime/AudioService.hpp"
#include <stdexcept>

namespace GLVM::core::Sound {
AudioService::AudioService(std::unique_ptr<ISoundEngine> backend)
    : backend_(std::move(backend)) {
    if (!backend_) throw std::invalid_argument("Audio backend is required");
}
AudioService::~AudioService() { stop(); }

void AudioService::start(const char* device) {
    if (opened_) return;
    backend_->OpenDevice(device);
    opened_ = true;
    error_ = nullptr;
    failed_ = false;
    running_ = true;
    try {
        worker_ = std::thread([this] {
            try {
                while (running_) backend_->SoundStream();
            } catch (...) {
                error_ = std::current_exception();
                failed_.store(true, std::memory_order_release);
                running_ = false;
            }
        });
    } catch (...) {
        stop();
        throw;
    }
}

void AudioService::stop() noexcept {
    running_ = false;
    backend_->RequestStop();
    if (worker_.joinable()) worker_.join();
    if (opened_) {
        // Device implementations must support shutdown without throwing.
        try { backend_->CloseDevice(); } catch (...) {}
        opened_ = false;
    }
}

void AudioService::rethrowWorkerError() const {
    if (failed_.load(std::memory_order_acquire)) std::rethrow_exception(error_);
}
}
