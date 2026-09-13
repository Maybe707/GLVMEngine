#pragma once
#include "Rendering/RenderAssets.hpp"
#include "Components/VertexComponent.hpp"
#include <deque>
#include <string>
namespace GLVM::core {
class AssetLibrary final {
public:
    ecs::TextureHandle LoadTextureFromFile(const char* path);
    ecs::TextureHandle LoadTextureFromAddress(unsigned int width, unsigned int height,
                                            unsigned int size, unsigned char* data);
    ecs::components::MeshHandle LoadMeshFromFile_OBJ(const char* path);
    ecs::components::MeshHandle LoadMeshFromFile_GLTF(const char* path);
    ecs::components::MeshHandle LoadMesh();
    void prepare();
    RenderAssets& data() noexcept { return assets_; }
    const RenderAssets& data() const noexcept { return assets_; }
    const vector<ecs::components::MeshHandle>& meshes() const noexcept { return meshHandlers; }
    const vector<ecs::TextureHandle>& textures() const noexcept { return textureHandlers; }
private:
    void requireUnprepared() const;
    const char* rememberPath(const char* path);
    void loadWavefrontObj();
    void initializeGLTF();
    void initializeFontData();
    void calculateMeshBounds(const vec4& vertex);
    bool isModelCacheExists(const std::string& path);
    void writeModelsCache(const std::string& path);
    RenderAssets assets_;
    std::deque<std::string> paths_;
    vector<ecs::components::MeshHandle> meshHandlers;
    vector<ecs::TextureHandle> textureHandlers;
    uint32_t meshID = 0;
    uint32_t wavefrontObjCounter = 0;
    bool isAlreadyCached = false;
    bool prepared_ = false;
};
}
