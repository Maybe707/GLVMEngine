// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef ENEMY_SYSTEM
#define ENEMY_SYSTEM

#include "ISystem.hpp"
#include "EntityManager.hpp"
#include "Vector.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/EnemyComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/StateComponent.hpp"
#include "ISoundEngine.hpp"

namespace GLVM::ecs
{
	class EnemySystem : public ISystem
	{
	public:
		void Update() override;
		void CalculateProjectile(unsigned int playerEntity, unsigned int enemyEntity);

		core::Sound::ISoundEngine* soundEngine;
		core::vector<ecs::TextureHandle> textureHandlers;
		core::vector<ecs::components::MeshHandle> meshHandlers;
		float projectileCooldown = 5.0f; 
		float deltaFrameTime;
	};
} // namespace GLVM::ecs

#endif
