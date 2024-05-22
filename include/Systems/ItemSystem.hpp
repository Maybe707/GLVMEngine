#ifndef ITEM_SYSTEM
#define ITEM_SYSTEM

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventsStack.hpp"
#include "ISystem.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/RigidBodyComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/ActorComponent.hpp"

namespace GLVM::ecs
{
	class ItemSystem : public ISystem
	{
	public:
		void Update();

		core::CStack* inputStack;
		bool          isInventoryOpened;
	};
} // namespace GLVM::ecs


#endif
