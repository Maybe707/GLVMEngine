#ifndef ITEM_SYSTEM
#define ITEM_SYSTEM

#include "Archetypes/ItemArchetype.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventsStack.hpp"
#include "ISystem.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ActorComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include <climits>
#include "ArchetypeECS/ArchetypeInterface.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "Archetypes/CrosshairArchetype.hpp"
#include "Archetypes/InventoryArchetype.hpp"
#include "ArchetypeECS/ArchECS_Utils.hpp"

namespace GLVM::ecs
{
	class ItemSystem : public ISystem
	{
	public:
		void Update();
		bool putItem2x2(components::inventory* inventoryComponent, unsigned int itemEntity);

		core::CStack* inputStack;
		bool          isInventoryOpened;
		int*          dragedItemEntity;
		bool*         isLeftMouseButtonReleased;
		bool          isLeftMouseButtonPressed;
		float         mouseOffsetX = 0;
		float         mouseOffsetY = 0;
		arch::Archetype* cachedInventoryArchetype;
		arch::Archetype* cachedItemArchetype;
		arch::Archetype* cachedCrosshairArchetype;
	};
} // namespace GLVM::ecs


#endif
