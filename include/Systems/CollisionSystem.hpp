// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "Systems/WorldSystem.hpp"
#include "Event.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ColliderFlagsComponent.hpp"
#include "VertexMath.hpp"
#include <cstdint>
#include "Common/CommonFunctions.hpp"
#include "Globals.hpp"

namespace GLVM::ecs
{
	class CCollisionSystem : public WorldSystem
	{   
	public:
		float fDelta_Time_ = {};
		float gravity = {};
		bool isInventoryOpened = {};
		bool* isItemDraged = nullptr;
		bool isLeftMouseButtonPressed = {};
		bool* isLeftMouseButtonReleased = nullptr;
        core::CStack& Input_Stack_;
		core::vector<ecs::arch::Archetype*> cachedArchetypes;
		uint32_t cachedArchetypesNumber = 0;
		
		struct CollisionComponentsView {
			components::transform* backtrackingTransforms        = nullptr;
			components::collider*  backtrackingColliders         = nullptr;
			components::colliderFlags* backtrackingColliderFlags = nullptr;
			components::mesh* backtrackingMeshes                 = nullptr;
			components::move* backtrackingMove = nullptr;
			components::transform* comparedTransforms            = nullptr;
			components::mesh*      comparedMeshes                = nullptr;
			components::move* comparedMove     = nullptr;
		} view;
		
		arch::componentMask	requiredMask = (1ul << arch::ComponentsIndices::COLLIDER_COMPONENT) |
			(1ul << arch::ComponentsIndices::COLLIDER_FLAGS_COMPONENT) |
			(1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT) |
			(1ul << arch::ComponentsIndices::MESH_COMPONENT);

        CCollisionSystem(arch::World& world, core::CStack& input, const core::vector<core::MeshAxisMaxAbsoluteValues>& bounds)
            : WorldSystem(world), Input_Stack_(input), bounds_(bounds) {}
        const core::vector<core::MeshAxisMaxAbsoluteValues>& bounds_;
		void Update() override;
        bool UpperActorCheck(vec3 backtrackingPosition,
							 vec3 comparedPosition,
							 float backtrackingScale,
							 float comparedScale,
							 components::MeshHandle backtrackingMeshHandle,
							 components::MeshHandle comparedMeshHandle);
    };
}
	
#endif
