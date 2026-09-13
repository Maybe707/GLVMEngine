#pragma once
#include "Assets/AssetLibrary.hpp"
#include "ProceduralLevelGeneratingSystem.hpp"
#include "Rendering/RenderFrame.hpp"
#include "SystemManager.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/DamageSystem.hpp"
#include "Systems/EnemySystem.hpp"
#include "Systems/InventorySystem.hpp"
#include "Systems/ItemSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/ProjectileSystem.hpp"
#include "Systems/SpatialGridSystem.hpp"

namespace GLVM::core {
struct GameplayFrame {
    float deltaTime;
    bool inventoryOpen;
    bool leftMousePressed;
    bool& leftMouseReleased;
    float pointerX, pointerY;
};

// Composition root for gameplay: ownership, order and per-frame inputs are kept
// together. Systems depend on services, never on the Engine singleton.
class GameplaySystems final {
public:
    GameplaySystems(ecs::arch::World& world, CStack& input, AssetLibrary& assets, Sound::ISoundEngine& audio,
                    int& draggedItem);
    void configureAssets(const AssetLibrary& assets);
    void update(const GameplayFrame& frame);
    void takeGeneratedGeometry(RenderAssets& destination);
private:
    CStack& input_;
    float gravity_ = 0.0f;
    ecs::CSystemManager scheduler_;
    ProceduralLevelGeneratingSystem& level_;
    ecs::CMovementSystem& movement_;
    ecs::EnemySystem& enemy_;
    ecs::CProjectileSystem& projectile_;
    ecs::SpatialGridSystem& spatial_;
    ecs::CCollisionSystem& collision_;
    ecs::DamageSystem& damage_;
    ecs::CPhysicsSystem& physics_;
    ecs::InventorySystem& inventory_;
    ecs::ItemSystem& item_;
};
}
