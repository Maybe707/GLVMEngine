#pragma once
#include "Rendering/RenderTypes.hpp"
#include "Texture.hpp"
#include <vector>
namespace GLVM::core {
struct RenderAssets {
        std::vector<ecs::Texture> initializeTextureData_{};
        std::vector<const char*> pathsArray_{};
		core::vector<const char*> pathsGLTF_{};
        std::vector<core::vector<core::Vertex>> levelGeneratedVertices{};
        std::vector<std::vector<uint32_t>> levelGeneratedIndices{};
        std::vector<core::vector<core::Vertex>> aVertices_{};
        std::vector<std::vector<uint32_t>> aIndices_{};
		std::vector<std::vector<float>> aVertexesTemp_{};
		std::vector<float> highest_gltf_Y{};
		MeshAxisLimitingValues meshAxisLimitingValues{};
		core::vector<core::vector<core::vector<mat4>>> jointMatricesPerMesh{};
		core::vector<core::vector<float>> frames{};
		core::vector<core::vector<Vertex>> symbolGVerticesContainer{};
		std::vector<unsigned int> fontIndicesContainer{};
};
}
