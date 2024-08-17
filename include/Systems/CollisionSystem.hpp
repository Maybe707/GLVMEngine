// This file is part of Game Loop Versatile Modules (GLVM)
// Copyright © 2024 Maksim Manokhin a.k.a. Yuriorkis_Scream. Contacts: <fellfrostqtw@gmail.com>
// Author: Maksim Manokhin a.k.a. Yuriorkis_Scream
// License: http://opensource.org/licenses/MIT

#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

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
#include <mutex>
#include "Globals.hpp"

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

        CCollisionSystem(core::CStack& _input_Stack) : Input_Stack_(_input_Stack) {}
		void Repel(components::transform& _transform_Component,
                   components::move& _move_Component,
				   float& _fDelta_Time,
				   components::beholder& _view_Component,
				   core::CEvent& _event);
        bool Gravity(components::transform& _transform_Component);
		bool BoxCollider(vec3 backtrackingPosition, vec3 comparedPosition,
		                 float backtrackingScale, float comparedScale);
		bool SquareCollider(vec3 backtrackingPosition, vec3 comparedPosition,
							float backtrackingScale, float comparedScale);
		bool DotCollider(vec3 backtrackingPosition, vec3 comparedPosition, float comparedScale);
		void Update() override;
		static core::vector<unsigned int> searchItemSlots(components::ItemSlotType itemSlotType, vec3 itemPosition, const core::vector<unsigned int>& collidedInventorySlotEntities,
													  const core::vector<vec3>& collidedInventorySlotTransforms);
		int areSlotsAvailable(const core::vector<unsigned int>& slots_);
		unsigned int searchMinimumValueIndex(core::vector<float> vector);
		void bubbleSortVector(core::vector<unsigned int>& vector_);
        bool UpperActorCheck(vec3 backtrackingPosition, vec3 comparedPosition,
							 float backtrackingScale, float comparedScale);
		bool RayCast(vec3 rayCasterPosition, vec3 receiverPosition,
					 float rayCasterScale, float receiverScale);
    };
}
	
#endif
