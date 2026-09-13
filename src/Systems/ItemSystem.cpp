#include "Systems/ItemSystem.hpp"

namespace GLVM::ecs {
bool ItemSystem::putItem2x2(components::inventory* inventory, unsigned int entity) {
    if (!inventory || entity >= world_.entityLocations.GetSize()) return false;
    const auto& location = world_.entityLocations[entity];
    if (!location.arch) return false;
    auto* items = static_cast<components::item*>(location.arch->components[arch::ComponentsIndices::ITEM_COMPONENT]);
    if (!items) return false;
    auto& item = items[location.index];
    const auto width = item.itemSlotType.width, height = item.itemSlotType.height;
    if (!width || !height || width > inventory->col || height > inventory->row) return false;
    for (unsigned int row = 0; row <= inventory->row - height; ++row) {
        for (unsigned int col = 0; col <= inventory->col - width; ++col) {
            bool available = true;
            for (unsigned int y = row; y < row + height; ++y)
                for (unsigned int x = col; x < col + width; ++x)
                    available &= inventory->slots[y][x] == UINT_MAX;
            if (!available) continue;
            item.occupiedSlots.clear();
            item.occupiedSlots.Reserve(width * height);
            for (unsigned int y = row; y < row + height; ++y)
                for (unsigned int x = col; x < col + width; ++x) {
                    inventory->slots[y][x] = entity;
                    item.occupiedSlots.Push(y * inventory->col + x);
                }
            return true;
        }
    }
    return false;
}
void ItemSystem::Update() {
    if (!isInventoryOpened) {
        for (auto* chunk : world_.query(itemRequiredMask)) {
            auto* items = static_cast<components::item*>(chunk->components[arch::ComponentsIndices::ITEM_COMPONENT]);
            auto* colliders = static_cast<components::collider*>(chunk->components[arch::ComponentsIndices::COLLIDER_COMPONENT]);
            for (unsigned int i = 0; i < chunk->entityCount; ++i) {
                for (auto* inventoriesChunk : world_.query(inventoryRequiredMask)) {
                    auto* inventories = static_cast<components::inventory*>(inventoriesChunk->components[arch::ComponentsIndices::INVENTORY_COMPONENT]);
                    for (unsigned int j = 0; j < inventoriesChunk->entityCount && items[i].isActor; ++j) {
                        const auto owner = inventories[j].entityOwner;
                        if (owner >= world_.entityLocations.GetSize() || !world_.entityLocations[owner].arch) continue;
                        const auto& ownerLocation = world_.entityLocations[owner];
                        const auto ownerEntity = ownerLocation.arch->entities[ownerLocation.index];
                        if (colliders[i].colliders.Find(ownerEntity).ValidStatus() &&
                            putItem2x2(&inventories[j], arch::getId(chunk->entities[i]))) {
                            items[i].isActor = false;
                        }
                    }
                }
            }
        }
    } else if (dragedItemEntity && *dragedItemEntity >= 0) {
        const auto id = static_cast<unsigned int>(*dragedItemEntity);
        if (id >= world_.entityLocations.GetSize()) return;
        const auto& itemLocation = world_.entityLocations[id];
        if (!itemLocation.arch || !arch::matchesRequiredMask(itemLocation.arch->mask, itemRequiredMask)) return;
        for (auto* chunk : world_.query(crosshairRequiredMask)) {
            if (!chunk->entityCount) continue;
            const auto* crosshair = static_cast<components::transform*>(chunk->components[arch::ComponentsIndices::TRANSFORM_COMPONENT]);
            auto* transforms = static_cast<components::transform*>(itemLocation.arch->components[arch::ComponentsIndices::TRANSFORM_COMPONENT]);
            transforms[itemLocation.index].position = crosshair[0].position;
            break; // One active pointer controls the local inventory.
        }
    }
}
}
