#ifndef INVENTORY_SYSTEM
#define INVENTORY_SYSTEM

#include "ISystem.hpp"

namespace GLVM::ecs
{
	class InventorySystem : public ecs::ISystem {
	public:
		void Update() override;

		bool          isInventoryOpened;
		bool          isItemDraged;
		bool*         isLeftMouseButtonReleased;
		bool          isLeftMouseButtonPressed;
		float           mouseOffsetX = 0;
		float           mouseOffsetY = 0;
	};
} ///< namespace GLVM::ecs


#endif
