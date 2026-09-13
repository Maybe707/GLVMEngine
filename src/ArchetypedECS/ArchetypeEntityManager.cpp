#include "ArchetypeECS/ArchetypeEntityManager.hpp"
#include <limits>

namespace GLVM::ecs::arch {
entity ArchetypeEntityManager::createEntity() {
    id newId;
    if (!freeList.empty()) {
        newId = freeList.GetHead();
        freeList.Pop();
    } else {
        if (generations.GetSize() == std::numeric_limits<id>::max())
            throw std::length_error("Entity IDs exhausted");
        newId = generations.GetSize();
        occupied.push_back(false);
        try { generations.Push(1); }
        catch (...) { occupied.pop_back(); throw; }
    }
    occupied[newId] = true;
    return makeEntity(newId, generations[newId]);
}
void ArchetypeEntityManager::removeEntity(entity value) {
    if (!isAlive(value)) return;
    const auto index = getId(value);
    // Retire IDs at generation exhaustion instead of resurrecting ancient handles.
    if (generations[index] != std::numeric_limits<generation>::max()) {
        freeList.Push(index);
        ++generations[index];
    }
    occupied[index] = false;
}
bool ArchetypeEntityManager::isAlive(entity value) const {
    const auto index = getId(value);
    return index < generations.GetSize() && occupied[index] && generations[index] == getGen(value);
}
}
