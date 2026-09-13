#include "Runtime/AudioService.hpp"
#include "SystemManager.hpp"
#include "Assets/AssetLibrary.hpp"
#include "GraphicAPI/RenderConfig.hpp"
#include "GraphicAPI/VkBuilders.hpp"
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace GLVM;

struct RecordingSystem final : ecs::ISystem {
    std::vector<int>& calls;
    int id;
    int& alive;
    RecordingSystem(std::vector<int>& calls, int id, int& alive)
        : calls(calls), id(id), alive(alive) { ++alive; }
    ~RecordingSystem() override { --alive; }
    void Update() override { calls.push_back(id); }
};

void schedulerTest() {
    std::vector<int> calls;
    int alive = 0;
    {
        ecs::CSystemManager scheduler;
        scheduler.add<RecordingSystem>(calls, 10, alive); // generator precedes movement
        auto& movement = scheduler.add<RecordingSystem>(calls, 20, alive);
        scheduler.add<RecordingSystem>(calls, 30, alive);
        scheduler.Update();
        assert((calls == std::vector<int>{10, 20, 30}));
        calls.clear();
        scheduler.setEnabled(movement, false);
        scheduler.setEnabled(movement, false);
        scheduler.Update();
        assert((calls == std::vector<int>{10, 30}));
        calls.clear();
        scheduler.setEnabled(movement, true);
        scheduler.Update();
        assert((calls == std::vector<int>{10, 20, 30}));
        ecs::CSystemManager other;
        try { other.setEnabled(movement, false); assert(false); }
        catch (const std::invalid_argument&) {}
    }
    assert(alive == 0);
    // Creating a second scheduler must not reuse a static count or old pointers.
    ecs::CSystemManager next;
    calls.clear();
    next.add<RecordingSystem>(calls, 40, alive);
    next.Update();
    assert((calls == std::vector<int>{40}));
}

struct AudioState {
    std::atomic<unsigned int> calls{0};
    std::atomic<bool> opened{false};
    std::atomic<bool> fail{false};
    unsigned int opens = 0, closes = 0;
    bool destroyed = false;
};
struct FakeAudio final : core::Sound::ISoundEngine {
    AudioState& state;
    core::vector<core::Sound::CSoundSample*> samples;
    explicit FakeAudio(AudioState& state) : state(state) {}
    ~FakeAudio() override { assert(!state.opened); state.destroyed = true; }
    void OpenDevice(const char*) override { assert(!state.opened); state.opened = true; ++state.opens; }
    void CloseDevice() override { assert(state.opened); state.opened = false; ++state.closes; }
    core::vector<core::Sound::CSoundSample*>& GetSoundContainer() override { return samples; }
    void PlaybackSoundSample(core::Sound::CSoundSample&) override {}
    void SetMasterVolume(long) override {}
    void CreateSoundSample(const char*, unsigned int, unsigned int, float) override {}
    void SoundStream() override {
        assert(state.opened);
        ++state.calls;
        if (state.fail) throw std::runtime_error("audio worker failure");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        assert(state.opened);
    }
};
template<class Predicate> void eventually(Predicate predicate) {
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!predicate()) {
        assert(std::chrono::steady_clock::now() < deadline);
        std::this_thread::yield();
    }
}
void audioTest() {
    AudioState state;
    {
        core::Sound::AudioService audio(std::make_unique<FakeAudio>(state));
        audio.start();
        audio.start();
        eventually([&] { return state.calls > 2; });
        audio.stop();
        audio.stop();
        assert(state.opens == 1 && state.closes == 1);
        const auto stoppedCount = state.calls.load();
        state.fail = true;
        audio.start();
        eventually([&] {
            try { audio.rethrowWorkerError(); return false; }
            catch (const std::runtime_error&) { return true; }
        });
        assert(state.calls > stoppedCount);
        // Destruction joins even after a worker exception; no explicit stop.
    }
    assert(state.destroyed && state.opens == 2 && state.closes == 2);
}

void resourceTest() {
    core::AssetLibrary library;
    {
        std::string temporary = "../gltf/simpleCube2.gltf";
        assert(library.LoadMeshFromFile_GLTF(temporary.c_str()).id == 0);
        temporary.assign(1000, 'x');
    }
    assert(std::strcmp(library.data().pathsGLTF_[0], "../gltf/simpleCube2.gltf") == 0);
    assert(library.meshes().GetSize() == 1);
    try { library.LoadMeshFromFile_GLTF(nullptr); assert(false); }
    catch (const std::invalid_argument&) {}
    assert(library.meshes().GetSize() == 1);
    core::AssetLibrary other;
    assert(other.LoadMeshFromFile_GLTF("another.gltf").id == 0);
}

void descriptorTest() {
    auto first = std::make_unique<core::VulkanResources>();
    auto second = std::make_unique<core::VulkanResources>();
    for (auto* state : {first.get(), second.get()}) {
        core::VkConfigInitializer(*state);
        core::descriptorSetBuilder(*state);
        core::pipelineBuilder(*state);
        core::renderPassesBuilder(*state);
    }
    assert(first->GPUDescriptors.GetSize() == second->GPUDescriptors.GetSize());
    assert(first->GPUDescriptors[0].GPUBuffer != second->GPUDescriptors[0].GPUBuffer);
    for (unsigned int i = 0; i < core::DESCRIPTOR_CHUNKS_NUMBER; ++i) {
        assert(first->descriptorSetsConfig[i].descriptorSetOffset == second->descriptorSetsConfig[i].descriptorSetOffset);
        assert(first->descriptorSetsConfig[i].descriptorsBindingsIDs[0] == second->descriptorSetsConfig[i].descriptorsBindingsIDs[0]);
    }
    first.reset();
    assert(second->GPUDescriptors[0].GPUBuffer->buffer == VK_NULL_HANDLE);
}

int main() {
    schedulerTest();
    std::cout << "PASS system order, pause/resume, independent schedulers, ownership\n";
    audioTest();
    std::cout << "PASS audio start/stop/restart, worker exception, destructor join\n";
    resourceTest();
    std::cout << "PASS resource path ownership and independent handle allocation\n";
    descriptorTest();
    std::cout << "PASS independent Vulkan descriptor tables and record ownership\n";
}
