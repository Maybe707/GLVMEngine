#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/ProjectileArchetype.hpp"
#include "Archetypes/PlayerArchetype.hpp"
#include "Archetypes/ItemArchetype.hpp"
#include "Archetypes/InventoryArchetype.hpp"
#include "Systems/SpatialGridSystem.hpp"
#include "Systems/DamageSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/ItemSystem.hpp"
#include "Systems/InventorySystem.hpp"
#include "Systems/EnemySystem.hpp"
#include <cassert>
#include <chrono>
#include <limits>
#include <memory>

using namespace GLVM;
namespace arch = ecs::arch;
namespace cm = ecs::components;

struct TinyChunk final : arch::Archetype {
    cm::transform transforms[2]{};
    cm::rotation rotations[2]{};
    cm::mesh meshes[2]{};
    cm::collider colliders[2]{};
    cm::colliderFlags flags[2]{};
    cm::health health[2]{};
    cm::attack attacks[2]{};
    cm::inventory inventories[2]{};
    TinyChunk() {
        registerComponent<arch::ComponentsIndices::TRANSFORM_COMPONENT>(transforms);
        registerComponent<arch::ComponentsIndices::ROTATION_COMPONENT>(rotations);
        registerComponent<arch::ComponentsIndices::MESH_COMPONENT>(meshes);
        registerComponent<arch::ComponentsIndices::COLLIDER_COMPONENT>(colliders);
        registerComponent<arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT>(flags);
        registerComponent<arch::ComponentsIndices::HEALTH_COMPONENT>(health);
        registerComponent<arch::ComponentsIndices::ATTACK_COMPONENT>(attacks);
        registerComponent<arch::ComponentsIndices::INVENTORY_COMPONENT>(inventories);
    }
    std::unique_ptr<arch::Archetype> cloneEmpty() const override { return std::make_unique<TinyChunk>(); }
};

template<class T> T* chunk(arch::World& world) {
    auto value = std::make_unique<T>();
    auto* result = value.get();
    world.archetypes.Push(result);
    value.release();
    return result;
}
arch::entity spawn(arch::World& world, arch::Archetype* target) {
    const auto value = world.entities.createEntity();
    world.addEntityToArchetype(value, target);
    return value;
}
void validateGrid(const arch::World& world) {
    unsigned int total = 0, memberships = 0;
    for (const auto& plane : world.spatialGrid.grid)
        for (const auto& row : plane)
            for (const auto& cell : row) {
                assert(cell.entities.GetSize() == cell.membershipIndices.GetSize());
                for (unsigned int i = 0; i < cell.entities.GetSize(); ++i) {
                    const auto& location = world.entityLocations[cell.entities[i]];
                    assert(location.arch);
                    assert(location.gridCells[cell.membershipIndices[i]].slot == i);
                    ++total;
                }
            }
    for (unsigned int id = 0; id < world.entityLocations.GetSize(); ++id)
        for (const auto& entry : world.entityLocations[id].gridCells) {
            assert(world.spatialGrid.grid[entry.z][entry.y][entry.x].entities[entry.slot] == id);
            ++memberships;
        }
    assert(total == memberships);
}
void chunksAndGrid() {
    auto world = std::make_unique<arch::World>();
    auto* seed = chunk<TinyChunk>(*world);
    try { world->addEntityToArchetype(arch::makeEntity(UINT_MAX, 1), seed); assert(false); }
    catch (const std::out_of_range&) {}
    std::vector<arch::entity> values;
    for (int i = 0; i < 100; ++i) {
        const auto entity = spawn(*world, seed);
        values.push_back(entity);
        const auto& location = world->entityLocations[arch::getId(entity)];
        auto* data = static_cast<TinyChunk*>(location.arch);
        assert(data->capacity == 2 && data->entityCount <= 2);
        data->rotations[location.index].yaw = static_cast<float>(i);
        data->inventories[location.index].slots[1][2] = i;
        data->health[location.index].currentHealth = 10;
        data->transforms[location.index].scale = 1;
    }
    assert(world->query(seed->mask).GetSize() == 50);
    arch::Archetype* small[1]{};
    unsigned int count = 0;
    try { world->searchCacheArchetypes(seed->mask, small, count); assert(false); }
    catch (const std::length_error&) { assert(!small[0]); }
    world->removeEntity(values[0]);
    assert(seed->entities[0] == values[1] && seed->rotations[0].yaw == 1);
    assert(seed->inventories[0].slots[1][2] == 1);
    const auto reused = spawn(*world, seed);
    assert(arch::getId(reused) == 0 && reused != values[0]);
    const auto index = world->entityLocations[0].index;
    assert(seed->inventories[index].slots[1][2] == UINT_MAX);
    seed->health[index].currentHealth = 10;
    seed->transforms[index].scale = 80; // Occupies 216 cells, beyond the old limit of 32.
    core::vector<core::MeshAxisMaxAbsoluteValues> bounds;
    bounds.Push({1, 1, 1, 0, 0, 0});
    ecs::SpatialGridSystem grid(*world, bounds);
    grid.Update();
    assert(grid.updatedEntities == 100 && world->entityLocations[0].gridCells.GetSize() > 32);
    validateGrid(*world);
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000; ++i) {
        grid.Update();
        assert(grid.updatedEntities == 0 && grid.visitedCells == 0);
    }
    std::cout << "Stationary grid, 100 entities / 1000 updates: "
              << std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count() << " ms\n";
    seed->transforms[index].position = vec3{510, 0, 0};
    grid.Update();
    assert(grid.updatedEntities == 1);
    validateGrid(*world);
    seed->transforms[index].position = vec3{10000, 0, 0};
    grid.Update();
    assert(world->entityLocations[0].gridCells.empty());
    seed->transforms[index].position[0] = std::numeric_limits<float>::quiet_NaN();
    grid.Update(); // Never convert NaN/negative/outside coordinates to an array index.
    validateGrid(*world);
    for (unsigned int i = 2; i < values.size(); i += 3) world->removeEntity(values[i]);
    validateGrid(*world);
    for (auto* arch : world->query(seed->mask)) {
        auto* data = static_cast<TinyChunk*>(arch);
        for (unsigned int i = 0; i < data->entityCount; ++i) data->attacks[i].damage = 100;
    }
    ecs::DamageSystem damage(*world);
    damage.Update(); // All adjacent dead objects, including swapped slots and entities without a font.
    for (auto* arch : world->archetypes) assert(arch->entityCount == 0);
    validateGrid(*world);
    assert(!world->entities.isAlive(reused));
    assert(!world->entities.isAlive(arch::makeEntity(0, arch::getGen(reused) + 1)));
    auto other = std::make_unique<arch::World>();
    assert(arch::getId(other->entities.createEntity()) == 0);
    std::cout << "PASS 50 chunks, typed swap/reset, inventory ownership, grid back-references, ID reuse and mass death\n";
}
void gameplayChunks() {
    auto world = std::make_unique<arch::World>();
    core::CStack input;
    ecs::CProjectileSystem projectiles(*world, input);
    ecs::CMovementSystem movement(*world, input);
    ecs::EnemySystem enemies(*world);
    ecs::ItemSystem items(*world);
    ecs::InventorySystem inventory(*world);
    projectiles.Update(); movement.Update(); enemies.Update(); items.Update();
    inventory.isInventoryOpened = true;
    inventory.Update();
    auto* seed = chunk<arch::ProjectileArchetype>(*world);
    for (unsigned int i = 0; i < seed->capacity * 3 + 1; ++i) {
        const auto value = spawn(*world, seed);
        const auto& location = world->entityLocations[arch::getId(value)];
        auto* data = static_cast<arch::ProjectileArchetype*>(location.arch);
        data->transforms[location.index].forward = vec3{0, 0, 1};
        data->heath[location.index].currentHealth = 100;
    }
    projectiles.deltaFrameTime = 0.1f;
    projectiles.Update();
    for (auto* arch : world->query(seed->mask)) {
        auto* data = static_cast<arch::ProjectileArchetype*>(arch);
        for (unsigned int i = 0; i < data->entityCount; ++i) assert(data->transforms[i].position[2] > 1);
    }
    auto* targetChunk = chunk<TinyChunk>(*world);
    const auto previousTarget = spawn(*world, targetChunk);
    world->removeEntity(previousTarget);
    const auto nextTarget = spawn(*world, targetChunk);
    assert(arch::getId(nextTarget) == arch::getId(previousTarget));
    seed->colliderFlags[0].flags = 1;
    seed->colliders[0].colliders.Push(previousTarget);
    seed->projectileBundles[0].damage.maximumDamage = 40;
    projectiles.Update();
    assert(targetChunk->attacks[0].damage == 0); // Previous-frame collision must not hit a reused ID.
    auto* player = chunk<arch::PlayerArchetype>(*world);
    for (unsigned int i = 0; i < player->capacity + 1; ++i) {
        const auto value = spawn(*world, player);
        const auto& location = world->entityLocations[arch::getId(value)];
        static_cast<cm::beholder*>(location.arch->components[arch::ComponentsIndices::VIEW_COMPONENT])[location.index].forward = vec3{0, 0, 1};
    }
    input.Push(core::EEvents::eMOVE_FORWARD);
    movement.deltaFrameTime = 0.1f;
    movement.Update();
    for (auto* arch : world->query(player->mask)) {
        const auto* moves = static_cast<cm::move*>(arch->components[arch::ComponentsIndices::MOVE_COMPONENT]);
        for (unsigned int i = 0; i < arch->entityCount; ++i) assert(moves[i].frameMovement[2] > 0);
    }
    std::cout << "PASS empty scenes, projectile movement and player input in overflow chunks\n";
}
void inventoryAndBounds() {
    auto world = std::make_unique<arch::World>();
    auto* seed = chunk<arch::ItemArchetype>(*world);
    const auto value = spawn(*world, seed);
    seed->items[0].itemSlotType = {1, 2};
    cm::inventory rectangular(2, 3);
    rectangular.slots[0] = {77, 77, 77};
    ecs::ItemSystem items(*world);
    assert(items.putItem2x2(&rectangular, arch::getId(value)));
    assert(rectangular.slots[1][0] == arch::getId(value) && rectangular.slots[1][1] == arch::getId(value));
    assert(seed->items[0].occupiedSlots[0] == 3);
    seed->items[0].itemSlotType = {3, 4};
    assert(!items.putItem2x2(&rectangular, arch::getId(value)));
    ecs::InventorySystem inventory(*world);
    core::vector<unsigned int> slots;
    assert(inventory.determineSwappableField(&seed->items[0], 4, 3, -1, 0, &rectangular, slots) == -2);
    rectangular.slots[0] = {0, 1, UINT_MAX}; // Entity zero is a valid occupied slot.
    assert(inventory.determineSwappableField(&seed->items[0], 2, 1, 0, 0, &rectangular, slots) == -2);
    const core::MeshAxisMaxAbsoluteValues offset{1, 2, 3, 10, 0, 0};
    const auto box = core::computeBoxCornerBoundPoints(offset, vec3{}, -2);
    assert(box[0][0] == -22 && box[1][0] == -18 && box[0][1] == -4);
    assert(core::BoxCollider(vec3{}, vec3{-20, 0, 0}, -2, 1, offset, {1, 1, 1, 0, 0, 0}));
    core::CStack input;
    core::vector<core::MeshAxisMaxAbsoluteValues> bounds;
    bounds.Push({1, 1, 1, 0, 0, 0});
    ecs::SpatialGridSystem grid(*world, bounds);
    ecs::CCollisionSystem collision(*world, input, bounds);
    seed->items[0].isActor = true;
    seed->transforms[0].scale = 1;
    grid.Update();
    assert(!world->entityLocations[arch::getId(value)].gridCells.empty());
    seed->items[0].isActor = false;
    grid.Update();
    assert(world->entityLocations[arch::getId(value)].gridCells.empty());
    grid.Update();
    assert(grid.updatedEntities == 0 && grid.visitedCells == 0);
    seed->items[0].isActor = true;
    grid.Update();
    assert(!world->entityLocations[arch::getId(value)].gridCells.empty());
    seed->transforms[0].position = vec3{-10000, 0, 0};
    grid.Update(); collision.Update();
    seed->meshes[0].handle.id = 999999;
    grid.Update(); collision.Update();
    std::cout << "PASS rectangular inventory, oversized items, entity zero, scaled bounds and outside-grid collisions\n";
}
int main() { chunksAndGrid(); gameplayChunks(); inventoryAndBounds(); }
