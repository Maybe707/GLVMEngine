#pragma once
#include "Texture.hpp"
#include "Components/VertexComponent.hpp"
#include <memory>

namespace GLVM::core {
// Application facade. Implementation details and platform headers stay private.
// One run per instance; GameKill is optional and idempotent (destruction stops it).
class Engine final {
public:
    Engine();
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    void GameLoop();
    void GameKill() noexcept;
    ecs::TextureHandle LoadTextureFromFile(const char* path);
    ecs::TextureHandle LoadTextureFromAddress(unsigned int width, unsigned int height,
                                            unsigned int size, unsigned char* data);
    ecs::components::MeshHandle LoadMeshFromFile_OBJ(const char* path);
    ecs::components::MeshHandle LoadMeshFromFile_GLTF(const char* path);
    ecs::components::MeshHandle LoadMesh();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
