#include "Systems/SpatialGridSystem.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Components/ItemComponent.hpp"

namespace GLVM::ecs {
void SpatialGridSystem::Update() {
    constexpr auto mask = (1ull << arch::ComponentsIndices::COLLIDER_COMPONENT) |
                          (1ull << arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
                          (1ull << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
                          (1ull << arch::ComponentsIndices::MESH_COMPONENT);
    updatedEntities = visitedCells = 0;
    for (auto* chunk : world_.query(mask)) {
        auto* transforms = static_cast<components::transform*>(chunk->components[arch::ComponentsIndices::TRANSFORM_COMPONENT]);
        auto* meshes = static_cast<components::mesh*>(chunk->components[arch::ComponentsIndices::MESH_COMPONENT]);
        auto* items = static_cast<components::item*>(chunk->components[arch::ComponentsIndices::ITEM_COMPONENT]);
        for (u32 i = 0; i < chunk->entityCount; ++i) {
            const auto id = arch::getId(chunk->entities[i]);
            auto& location = world_.entityLocations[id];
            if (items && !items[i].isActor) {
                if (location.gridInitialized || location.isDirty) {
                    visitedCells += location.gridCells.GetSize();
                    world_.detachSpatial(id);
                    location.isDirty = false;
                    ++updatedEntities;
                }
                continue;
            }
            auto& transform = transforms[i];
            const auto mesh = meshes[i].handle.id;
            // Also compare transforms: legacy systems may forget to mark an entity dirty.
            if (location.gridInitialized && !location.isDirty && location.gridPosition == transform.position &&
                location.gridScale == transform.scale && location.gridMesh == mesh) continue;
            visitedCells += location.gridCells.GetSize();
            world_.detachSpatial(id);
            if (mesh < bounds_.GetSize()) {
                const auto& bounds = bounds_[mesh];
                const vec3 center(bounds.origin_offset_x, bounds.origin_offset_y, bounds.origin_offset_z);
                const vec3 extent(bounds.absolute_x, bounds.absolute_y, bounds.absolute_z);
                vec3 low = center - extent;
                vec3 high = center + extent;
                for (unsigned int axis = 0; axis < 3; ++axis) {
                    const float a = low[axis] * transform.scale + transform.position[axis];
                    const float b = high[axis] * transform.scale + transform.position[axis];
                    low[axis] = std::min(a, b);
                    high[axis] = std::max(a, b);
                }
                if (auto range = arch::SpatialGrid::cellRange(low, high)) world_.attachSpatial(id, *range);
            }
            visitedCells += location.gridCells.GetSize();
            location.gridPosition = transform.position;
            location.gridScale = transform.scale;
            location.gridMesh = mesh;
            location.gridInitialized = true;
            location.isDirty = false;
            ++updatedEntities;
        }
    }
}
}
