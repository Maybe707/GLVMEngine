// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "Components/ItemComponent.hpp"
#include "Vector.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/VertexComponent.hpp"
#include "ISystem.hpp"
#include "Event.hpp"
#include "Components/MoveComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Vector.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <cstdint>
#include <mutex>
#include "Globals.hpp"
#include "GraphicAPI/Vulkan.hpp"

namespace GLVM::ecs
{
	class CCollisionSystem : public ISystem
	{   
	public:
        
		float fDelta_Time_;
		float gravity;
		bool isInventoryOpened;
		bool* isItemDraged;
		bool isLeftMouseButtonPressed;
		bool* isLeftMouseButtonReleased;
        core::CStack& Input_Stack_;
		arch::Archetype* cachedArchetypes[32];
		uint32_t cachedArchetypesNumber = 0;

        CCollisionSystem(core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
		void Repel(components::transform& _transform_Component,
                   components::move& _move_Component,
				   float& _fDelta_Time,
				   components::beholder& _view_Component,
				   core::CEvent& _event);
        bool Gravity(components::transform& _transform_Component);
		bool BoxCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                 float backtrackingScale, float comparedScale,
						 components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle);
		bool SquareCollider(vec3 backtrackingPosition, vec3 comparedPosition,
							float backtrackingScale, float comparedScale_X, float comparedScale_Y,
							components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle);
		bool DotCollider(vec3 backtrackingPosition, vec3 comparedPosition, float comparedScale,
						 components::MeshHandle comparedMeshHandle);
		void Update() override;
		static core::vector<unsigned int> searchItemSlots(components::ItemSlotType itemSlotType, vec3 itemPosition, const core::vector<unsigned int>& collidedInventorySlotEntities,
													  const core::vector<vec3>& collidedInventorySlotTransforms);
		int slotsAvailabilityState(const core::vector<unsigned int>& slots_);
		unsigned int searchMinimumValueIndex(core::vector<float> vector);
		void bubbleSortVector(core::vector<unsigned int>& vector_);
        bool UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
							 float backtrackingScale, float comparedScale,
							 components::MeshHandle backtrackingMeshHandle, components::MeshHandle comparedMeshHandle);
		bool RayCast(vec3 rayCasterPosition, vec3 receiverPosition,
					 float rayCasterScale, float receiverScale);
    };
}
	
#endif
