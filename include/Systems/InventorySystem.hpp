#ifndef INVENTORY_SYSTEM
#define INVENTORY_SYSTEM

#include "ISystem.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/CrosshairComponent.hpp"

namespace GLVM::ecs
{
	class InventorySystem : public ecs::ISystem {
	public:
		void Update() override;

		bool          isInventoryOpened;
		bool          isItemDraged;
		bool*         isLeftMouseButtonReleased;
		bool          isLeftMouseButtonPressed;
		float         mouseOffsetX = 0;
		float         mouseOffsetY = 0;
		float         aspectRate   = 1.778;                   ///< Multiplier of current aspect rate. For full hd this must be 1920 / 1080
	};
} ///< namespace GLVM::ecs


#endif
