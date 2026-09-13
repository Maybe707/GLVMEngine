#include "Runtime/GameplaySystems.hpp"

namespace GLVM::core {
GameplaySystems::GameplaySystems(CStack& input, AssetLibrary& assets,
                                 Sound::ISoundEngine& audio, int& draggedItem)
    : input_(input),
      level_(scheduler_.add<ProceduralLevelGeneratingSystem>(assets)),
      movement_(scheduler_.add<ecs::CMovementSystem>(input)),
      enemy_(scheduler_.add<ecs::EnemySystem>()),
      projectile_(scheduler_.add<ecs::CProjectileSystem>(input)),
      spatial_(scheduler_.add<ecs::SpatialGridSystem>()),
      collision_(scheduler_.add<ecs::CCollisionSystem>(input)),
      damage_(scheduler_.add<ecs::DamageSystem>()),
      physics_(scheduler_.add<ecs::CPhysicsSystem>(gravity_, input)),
      inventory_(scheduler_.add<ecs::InventorySystem>()),
      item_(scheduler_.add<ecs::ItemSystem>()) {
    enemy_.soundEngine = &audio;
    projectile_.soundEngine = &audio;
    inventory_.isItemDraged = &draggedItem;
    item_.dragedItemEntity = &draggedItem;
    item_.inputStack = &input_;
}

void GameplaySystems::configureAssets(const AssetLibrary& assets) {
    projectile_.textureHandlers = assets.textures();
    projectile_.meshHandlers = assets.meshes();
    enemy_.textureHandlers = assets.textures();
    enemy_.meshHandlers = assets.meshes();
    level_.textureHandlers = assets.textures();
    level_.meshHandlers = assets.meshes();
}

void GameplaySystems::update(const GameplayFrame& frame) {
    gravity_ += frame.deltaTime;
    scheduler_.setEnabled(movement_, !frame.inventoryOpen);
    damage_.deltaTime = frame.deltaTime;
    movement_.deltaFrameTime = frame.deltaTime;
    movement_.gravity = gravity_;
    collision_.fDelta_Time_ = frame.deltaTime;
    collision_.gravity = gravity_;
    collision_.isInventoryOpened = frame.inventoryOpen;
    collision_.isLeftMouseButtonPressed = frame.leftMousePressed;
    collision_.isLeftMouseButtonReleased = &frame.leftMouseReleased;
    enemy_.deltaFrameTime = frame.deltaTime;
    projectile_.deltaFrameTime = frame.deltaTime;
    projectile_.isInventoryOpened = frame.inventoryOpen;
    physics_.fDelta_Time_ = frame.deltaTime;
    physics_.fAcceleration_of_Gravity_ += frame.deltaTime / 20;
    physics_.gravity = gravity_;
    inventory_.isInventoryOpened = frame.inventoryOpen;
    inventory_.isLeftMouseButtonReleased = &frame.leftMouseReleased;
    inventory_.isLeftMouseButtonPressed = frame.leftMousePressed;
    inventory_.mouseOffsetX = frame.pointerX;
    inventory_.mouseOffsetY = frame.pointerY;
    item_.isInventoryOpened = frame.inventoryOpen;
    item_.isLeftMouseButtonReleased = &frame.leftMouseReleased;
    item_.isLeftMouseButtonPressed = frame.leftMousePressed;
    item_.mouseOffsetX = frame.pointerX;
    item_.mouseOffsetY = frame.pointerY;
    scheduler_.Update();
}

void GameplaySystems::takeGeneratedGeometry(RenderAssets& destination) {
    destination.levelGeneratedVertices = std::move(level_.levelGeneratedVertices);
    destination.levelGeneratedIndices = std::move(level_.levelGeneratedIndices);
    level_.levelGeneratedVertices.clear();
    level_.levelGeneratedIndices.clear();
}
}
