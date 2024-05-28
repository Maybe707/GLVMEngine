#ifndef ITEM_SYSTEM
#define ITEM_SYSTEM

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

namespace GLVM::ecs
{
	class ItemSystem : public ISystem
	{
	public:
		void Update();
		bool putItem2x2(components::inventory* inventoryComponent, unsigned int itemEntity);

		core::CStack* inputStack;
		bool          isInventoryOpened;
		bool          isItemDraged;
		bool*         isLeftMouseButtonReleased;
		bool          isLeftMouseButtonPressed;
	};
} // namespace GLVM::ecs


#endif
