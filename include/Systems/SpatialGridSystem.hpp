#pragma once
#include "ISystem.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Globals.hpp"

namespace GLVM::ecs {
class SpatialGridSystem : public ISystem {
    arch::World& world_;
    const core::vector<core::MeshAxisMaxAbsoluteValues>& bounds_;
public:
    explicit SpatialGridSystem(arch::World& world,
                               const core::vector<core::MeshAxisMaxAbsoluteValues>& bounds)
        : world_(world), bounds_(bounds) {}
    u32 updatedEntities = 0;
    u32 visitedCells = 0;
    void Update() override;
};
}
