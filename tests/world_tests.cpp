#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/StaticMeshArchetype.hpp"
#include <memory>

int main() {
    namespace arch = GLVM::ecs::arch;
    auto world = std::make_unique<arch::World>();
    auto* meshes = new arch::StaticMeshArchetype;
    world->archetypes.Push(meshes);
    auto a = arch::makeEntity(0, 1), b = arch::makeEntity(1, 1);
    world->addEntityToArchetype(a, meshes);
    world->addEntityToArchetype(b, meshes);
    meshes->rotations[1].yaw = 42;
    world->removeEntity(arch::makeEntity(100000, 1));
    world->removeEntity(arch::makeEntity(0, 2));
    assert(meshes->entityCount == 2);
    world->removeEntity(a);
    world->removeEntity(a);
    assert(meshes->entityCount == 1 && meshes->entities[0] == b);
    assert(meshes->rotations[0].yaw == 42 && world->entityLocations[1].index == 0);
    arch::Archetype* cache[1]{};
    uint32_t count = 0;
    world->searchCacheArchetypes(meshes->mask, cache, count);
    assert(count == 1 && cache[0] == meshes);
    while (meshes->entityCount < meshes->capacity) meshes->addEntity(b);
    bool full = false;
    try { meshes->addEntity(b); } catch (const std::length_error&) { full = true; }
    assert(full && meshes->capacity == arch::STATIC_MESH_ARCH_CHUNK_SIZE);
    std::cout << "PASS ECS removal, rotation, local world lookup and component capacity\n";
}
