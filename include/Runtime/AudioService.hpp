#pragma once

#include "ISoundEngine.hpp"
#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace GLVM::core::Sound {

// The worker never outlives its device. Worker errors are reported on the main
// thread instead of allowing an exception to terminate the process.
class AudioService final {
public:
    explicit AudioService(std::unique_ptr<ISoundEngine> backend);
    ~AudioService();
    AudioService(const AudioService&) = delete;
    AudioService& operator=(const AudioService&) = delete;
    void start(const char* device = "default");
    void stop() noexcept;
    void rethrowWorkerError() const;
    ISoundEngine& backend() noexcept { return *backend_; }

private:
    std::unique_ptr<ISoundEngine> backend_;
    std::atomic<bool> running_{false};
    std::atomic<bool> failed_{false};
    std::exception_ptr error_;
    std::thread worker_;
    bool opened_ = false;
};
}
