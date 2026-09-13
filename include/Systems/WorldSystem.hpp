#pragma once
#include "ISystem.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"

namespace GLVM::ecs {
class WorldSystem : public ISystem {
protected:
    arch::World& world_;
    explicit WorldSystem(arch::World& world) : world_(world) {}
};
}
