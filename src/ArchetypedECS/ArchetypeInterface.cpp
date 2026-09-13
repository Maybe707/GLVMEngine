#include "ArchetypeECS/ArchetypeInterface.hpp"
#include <stdexcept>

namespace GLVM::ecs::arch {
uint32_t Archetype::addEntity(entity value) {
    if (entityCount >= capacity) throw std::length_error("Archetype capacity exceeded");
    for (uint32_t i = 0; i < componentCount; ++i)
        reset_[componentIds[i]](components[componentIds[i]], entityCount);
    const uint32_t index = entityCount++;
    entities[index] = value;
    return index;
}
entity Archetype::removeEntity(uint32_t index) {
    if (index >= entityCount) throw std::out_of_range("Invalid archetype entity index");
    const uint32_t last = entityCount - 1;
    for (uint32_t i = 0; i < componentCount; ++i) {
        const auto component = componentIds[i];
        if (index != last) move_[component](components[component], index, last);
        reset_[component](components[component], last);
    }
    const entity moved = entities[last];
    entities[index] = moved;
    entities[last] = 0;
    --entityCount;
    return moved;
}
}
