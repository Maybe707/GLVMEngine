#include "Systems/CollisionSystem.hpp"
#include "Components/ItemComponent.hpp"
#include <algorithm>

namespace GLVM::ecs {
void CCollisionSystem::Update() {
    const auto predicted = [](const components::transform& transform, const components::move* move) {
        return move ? transform.position + move->frameMovement + move->gravity : transform.position;
    };
    for (auto* chunk : world_.query(requiredMask)) {
        auto* transforms = static_cast<components::transform*>(chunk->components[arch::ComponentsIndices::TRANSFORM_COMPONENT]);
        auto* meshes = static_cast<components::mesh*>(chunk->components[arch::ComponentsIndices::MESH_COMPONENT]);
        auto* colliders = static_cast<components::collider*>(chunk->components[arch::ComponentsIndices::COLLIDER_COMPONENT]);
        auto* flags = static_cast<components::colliderFlags*>(chunk->components[arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT]);
        auto* moves = static_cast<components::move*>(chunk->components[arch::ComponentsIndices::MOVE_COMPONENT]);
        auto* items = static_cast<components::item*>(chunk->components[arch::ComponentsIndices::ITEM_COMPONENT]);
        for (u32 i = 0; i < chunk->entityCount; ++i) {
            colliders[i].colliders.clear();
            flags[i].flags &= ~3u;
            if (items && !items[i].isActor) continue;
            if (meshes[i].handle.id >= bounds_.GetSize()) continue;
            const auto position = predicted(transforms[i], moves ? &moves[i] : nullptr);
            const auto box = core::computeBoxCornerBoundPoints(bounds_[meshes[i].handle.id], position, transforms[i].scale);
            const auto range = arch::SpatialGrid::cellRange(box[0], box[1]);
            if (!range) continue;
            core::vector<u32> candidates;
            for (u32 z = range->minZ; z <= range->maxZ; ++z)
                for (u32 y = range->minY; y <= range->maxY; ++y)
                    for (u32 x = range->minX; x <= range->maxX; ++x)
                        for (const auto entity : world_.spatialGrid.grid[z][y][x].entities)
                            candidates.Push(entity);
            std::sort(candidates.begin(), candidates.end());
            const auto end = std::unique(candidates.begin(), candidates.end());
            for (auto it = candidates.begin(); it != end; ++it) {
                const auto id = *it;
                if (id == arch::getId(chunk->entities[i]) || id >= world_.entityLocations.GetSize()) continue;
                const auto& location = world_.entityLocations[id];
                if (!location.arch || !arch::matchesRequiredMask(location.arch->mask, requiredMask)) continue;
                const auto j = location.index;
                const auto* other = location.arch;
                const auto& transform = static_cast<components::transform*>(other->components[arch::ComponentsIndices::TRANSFORM_COMPONENT])[j];
                const auto mesh = static_cast<components::mesh*>(other->components[arch::ComponentsIndices::MESH_COMPONENT])[j].handle;
                if (mesh.id >= bounds_.GetSize()) continue;
                const auto* otherMoves = static_cast<components::move*>(other->components[arch::ComponentsIndices::MOVE_COMPONENT]);
                const auto otherPosition = predicted(transform, otherMoves ? &otherMoves[j] : nullptr);
                if (!core::BoxCollider(position, otherPosition, transforms[i].scale, transform.scale,
                                       bounds_[meshes[i].handle.id], bounds_[mesh.id])) continue;
                flags[i].flags |= UpperActorCheck(position, otherPosition, transforms[i].scale,
                                                  transform.scale, meshes[i].handle, mesh) ? 2u : 1u;
                colliders[i].colliders.Push(other->entities[j]);
            }
        }
    }
}
bool CCollisionSystem::UpperActorCheck(vec3 a, vec3 b, float scaleA, float scaleB,
                                      components::MeshHandle meshA, components::MeshHandle meshB) {
    if (meshA.id >= bounds_.GetSize() || meshB.id >= bounds_.GetSize()) return false;
    const auto boxA = core::computeBoxCornerBoundPoints(bounds_[meshA.id], a, scaleA);
    const auto boxB = core::computeBoxCornerBoundPoints(bounds_[meshB.id], b, scaleB);
    return boxA[0][1] + 0.15f > boxB[1][1];
}
}
