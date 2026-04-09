#ifndef PROJECTILE_BUNDLE_HPP
#define PROJECTILE_BUNDLE_HPP

#include "Components/ProjectileComponent.hpp"
#include "Components/DamageComponent.hpp"
#include "Components/MaterialComponent.hpp"

namespace GLVM::ecs::arch {
	struct ProjectileBundle {
		components::projectile projectile;
		components::damage     damage;
		components::material   material;
	};
}; // namespace GLVM::ecs::components

#endif
