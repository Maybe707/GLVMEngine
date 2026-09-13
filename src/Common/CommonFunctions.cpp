// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#include "Common/CommonFunctions.hpp"
#include "Components/ColliderComponent.hpp"

namespace GLVM::core {
    bool BoxCollider(vec3 a, vec3 b, float scaleA, float scaleB,
                     const MeshAxisMaxAbsoluteValues& boundsA, const MeshAxisMaxAbsoluteValues& boundsB) {
        const auto boxA = computeBoxCornerBoundPoints(boundsA, a, scaleA);
        const auto boxB = computeBoxCornerBoundPoints(boundsB, b, scaleB);
        for (unsigned int axis = 0; axis < 3; ++axis)
            if (!(boxA[1][axis] > boxB[0][axis] && boxA[0][axis] < boxB[1][axis])) return false;
        return true;
    }
    std::array<vec3, 2> computeBoxCornerBoundPoints(MeshAxisMaxAbsoluteValues bounds, vec3 position, float scale) {
        const vec3 center = position + vec3(bounds.origin_offset_x, bounds.origin_offset_y, bounds.origin_offset_z) * scale;
        const vec3 extent = vec3(bounds.absolute_x, bounds.absolute_y, bounds.absolute_z) * std::abs(scale);
        return {center - extent, center + extent};
    }

    MeshAxisMaxAbsoluteValues calculateBounds(MeshAxisLimitingValues values) {
        MeshAxisMaxAbsoluteValues bounds{};
        bounds.absolute_x = (values.highest_x - values.lowest_x) * 0.5f;
        bounds.absolute_y = (values.highest_y - values.lowest_y) * 0.5f;
        bounds.absolute_z = (values.highest_z - values.lowest_z) * 0.5f;
        bounds.origin_offset_x = (values.highest_x + values.lowest_x) * 0.5f;
        bounds.origin_offset_y = (values.highest_y + values.lowest_y) * 0.5f;
        bounds.origin_offset_z = (values.highest_z + values.lowest_z) * 0.5f;
        return bounds;
    }

	void CreateProjectile(const vec3& projectilePosition,
							 const vec3& projectileForward,
							 const ecs::components::MeshHandle& meshHandle,
							 const ecs::components::material& material,
							 const ecs::components::damage& damage,
							 const ecs::arch::EntityLocation& projectileLocation) {
		ecs::arch::ProjectileArchetype* projectileArch = static_cast<ecs::arch::ProjectileArchetype*>(projectileLocation.arch);
		const uint32_t projectileIndex = projectileLocation.index;
		
		ecs::components::mesh* projectileMesh = &projectileArch->meshes[projectileIndex];
		projectileMesh->handle = meshHandle;

		ecs::arch::ProjectileBundle* projectileBundle = &projectileArch->projectileBundles[projectileIndex];
		projectileBundle->material  = material;
		
		ecs::components::transform* rTransformProjectile = &projectileArch->transforms[projectileIndex];
		ecs::components::health*    projectileHealth     = &projectileArch->heath[projectileIndex];
		projectileHealth->maxHealth     = 100;
		projectileHealth->currentHealth = 100;

		projectileArch->colliders[projectileIndex].colliders.clear();

		rTransformProjectile->scale = 0.3f;
		rTransformProjectile->position = projectilePosition;
		rTransformProjectile->forward   = projectileForward;
		rTransformProjectile->position += rTransformProjectile->forward;
		
		projectileBundle->damage = damage;
	}
}; ///< namespace GLVM::core
