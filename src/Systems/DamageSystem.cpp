#include "Systems/DamageSystem.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "ArchetypeECS/ArchetypeEntityManager.hpp"

namespace GLVM::ecs {
void DamageSystem::Update() {
    for (auto* chunk : world_.query(attackableRequiredMask)) {
        auto* health = static_cast<components::health*>(chunk->components[arch::ComponentsIndices::HEALTH_COMPONENT]);
        auto* attacks = static_cast<components::attack*>(chunk->components[arch::ComponentsIndices::ATTACK_COMPONENT]);
        auto* fonts = static_cast<components::font*>(chunk->components[arch::ComponentsIndices::FONT_COMPONENT]);
        for (u32 i = 0; i < chunk->entityCount;) {
            const auto damage = attacks[i].damage;
            health[i].currentHealth -= damage;
            attacks[i].damage = 0;
            if (health[i].currentHealth <= 0) {
                const auto entity = chunk->entities[i];
                world_.removeEntity(entity);
                continue; // Swap-remove placed another entity at this index.
            }
            if (damage && fonts) {
                fonts[i].font_string.clear();
                for (char c : std::to_string(damage)) fonts[i].font_string.Push(c);
                fonts[i].lifeTime = 0;
                fonts[i].removeble = true;
            }
            ++i;
        }
    }
    for (auto* chunk : world_.query(fontRequiredMask)) {
        auto* fonts = static_cast<components::font*>(chunk->components[arch::ComponentsIndices::FONT_COMPONENT]);
        for (u32 i = 0; i < chunk->entityCount; ++i)
            if (fonts[i].removeble) {
                fonts[i].lifeTime += deltaTime;
                if (fonts[i].lifeTime >= 1.5f) fonts[i].font_string.clear();
            }
    }
}
}
