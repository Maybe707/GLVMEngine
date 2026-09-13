#include "Engine.hpp"
#include "Assets/AssetLibrary.hpp"
#include "GraphicAPI/Vulkan.hpp"
#include "Rendering/ScenePresenter.hpp"
#include "Runtime/AudioService.hpp"
#include "Runtime/GameplaySystems.hpp"
#include "SoundEngineFactory.hpp"
#include "TimerCreator.hpp"
#include "Globals.hpp"
#include <charconv>
#include <chrono>
#include <cstdlib>
#include <string_view>

namespace GLVM::core {
namespace {
unsigned long frameLimitFromEnvironment() {
    const char* text = std::getenv("GLVM_MAX_FRAMES");
    if (!text) return 0;
    unsigned long limit = 0;
    const std::string_view value(text);
    auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), limit);
    if (error != std::errc{} || end != value.data() + value.size())
        throw std::invalid_argument("GLVM_MAX_FRAMES must be a non-negative integer");
    return limit;
}
}

struct Engine::Impl {
    enum class State { Ready, Running, Stopped };
    State state = State::Ready;
    AssetLibrary assets;
    RenderFrame frame;
    std::unique_ptr<Time::IChrono> timer{Time::CTimerCreator().Create()};
    Sound::AudioService audio{std::unique_ptr<Sound::ISoundEngine>(Sound::CSoundEngineFactory().CreateSoundEngine())};
    GameplaySystems systems{Input_Stack_, assets, audio.backend(), frame.dragedItemEntity};
    std::unique_ptr<ScenePresenter> scene;
    std::unique_ptr<CVulkanRenderer> renderer;

    void stop() noexcept {
        renderer.reset();
        scene.reset();
        audio.stop();
        state = State::Stopped;
    }

    void run() {
        if (state != State::Ready) throw std::logic_error("Engine supports one run per instance");
        const auto limit = frameLimitFromEnvironment();
        const auto startup = std::chrono::steady_clock::now();
        state = State::Running;
        assets.prepare();
        systems.configureAssets(assets);
        scene = std::make_unique<ScenePresenter>(ecs::arch::world, assets.data(), frame, g_eEvent, Input_Stack_);
        scene->prepareFrame();
        renderer = std::make_unique<CVulkanRenderer>(assets.data(), frame);
        renderer->run();
        audio.start();
        const auto ready = std::chrono::steady_clock::now();
        std::cout << "Vulkan ready in " << std::chrono::duration<double>(ready - startup).count() << " seconds" << std::endl;
        timer->Reset();
        unsigned long renderedFrames = 0;
        auto& window = renderer->window();
        while (!limit || renderedFrames < limit) {
            const float seconds = std::min(timer->GetElapsed(), 0.1);
            timer->Reset();
            audio.rethrowWorkerError();
            window.ClearDisplay();
            g_eEvent.SetEvent(EEvents::eDEFAULT);
            window.HandleEvent(g_eEvent);
            const auto pressed = [](EEvents key) { return Input_Stack_.SearchElement(key) == key; };
            if (pressed(EEvents::eGAME_LOOP_KILL)) break;
            if (pressed(EEvents::eINVENTORY)) {
                frame.isInventoryOpened = !frame.isInventoryOpened;
                std::cout << "Inventory: " << (frame.isInventoryOpened ? "open" : "closed") << std::endl;
                Input_Stack_.Remove(EEvents::eINVENTORY);
            }
            if (pressed(EEvents::eDEBUG_COLLISIONS_ACTIVE)) {
                frame.isDebugCollisitionsActive = !frame.isDebugCollisitionsActive;
                std::cout << "Collisions: " << (frame.isDebugCollisitionsActive ? "on" : "off") << std::endl;
                Input_Stack_.Remove(EEvents::eDEBUG_COLLISIONS_ACTIVE);
            }
            g_eEvent.SetLastEvent(Input_Stack_);
            window.CursorLock(g_eEvent.mousePointerPosition.position_X,
                              g_eEvent.mousePointerPosition.position_Y,
                              &g_eEvent.mousePointerPosition.offset_X,
                              &g_eEvent.mousePointerPosition.offset_Y);
            frame.aspectRate = static_cast<float>(window.width) / std::max(1u, static_cast<unsigned>(window.height));
            scene->updatePointer();
            scene->advanceAnimations(seconds);
            systems.update({seconds, frame.isInventoryOpened, pressed(EEvents::eMOUSE_LEFT_BUTTON),
                            g_eEvent.isLeftMouseButtonReleased, frame.hud_screen_x, frame.hud_screen_y});
            systems.takeGeneratedGeometry(assets.data());
            renderer->initializeGameLevelVertices();
            scene->prepareFrame();
            renderer->draw();
            window.SwapBuffers();
            ++renderedFrames;
            if (limit && renderedFrames % 30 == 0)
                std::cout << "Frame " << renderedFrames << "/" << limit << std::endl;
        }
        if (limit) std::cout << "Rendered " << renderedFrames << " frames" << std::endl;
        const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - ready).count();
        if (limit && elapsed > 0) std::cout << "Average FPS: " << renderedFrames / elapsed << std::endl;
        stop();
    }
};

Engine::Engine() : impl_(std::make_unique<Impl>()) {}
Engine::~Engine() = default;
void Engine::GameLoop() {
    try { impl_->run(); }
    catch (...) { impl_->stop(); throw; }
}
void Engine::GameKill() noexcept { impl_->stop(); }
ecs::TextureHandle Engine::LoadTextureFromFile(const char* path) {
    return impl_->assets.LoadTextureFromFile(path);
}
ecs::TextureHandle Engine::LoadTextureFromAddress(unsigned int width, unsigned int height,
                                                unsigned int size, unsigned char* data) {
    return impl_->assets.LoadTextureFromAddress(width, height, size, data);
}
ecs::components::MeshHandle Engine::LoadMeshFromFile_OBJ(const char* path) {
    return impl_->assets.LoadMeshFromFile_OBJ(path);
}
ecs::components::MeshHandle Engine::LoadMeshFromFile_GLTF(const char* path) {
    return impl_->assets.LoadMeshFromFile_GLTF(path);
}
ecs::components::MeshHandle Engine::LoadMesh() { return impl_->assets.LoadMesh(); }
}
