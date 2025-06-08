#ifndef INVENTORY_SYSTEM
#define INVENTORY_SYSTEM

#include "ISystem.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/CrosshairComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "VertexMath.hpp"
#include "Components/TransformComponent.hpp"
#include <print>

namespace GLVM::ecs
{
	class InventorySystem : public ecs::ISystem {
	public:
		void Update() override;
		int determineSwappableStatusAndSlots( components::item* itemComponent, components::transform* inventoryTransformComponent,
											  core::vector<unsigned int>& potentialOccupiedSlots, components::transform* crosshairTransformComponent,
											  point2D<int> intersectionSlot, components::inventory* inventoryComponent, const float inventorySlotScale );
		void fillInventorySlots( components::item* itemComponent, const int itemWidth, const int itemHeight,
								 components::inventory* inventoryComponent, const int fillValue );
		int determineSwappableField( components::item* itemComponent, const int itemWidth, const int itemHeight,
									 int pivotRow, int pivotColumn, components::inventory* inventoryComponent,
									 core::vector<unsigned int>& potentialOccupiedSlots );
		int calculateBasicOffset( const int itemAxisSize, const float axisValue,
								  const float crosshairAxisPosition, const int axisSlotIndex,
								  const float inventorySlotScale );
		bool checkCrosshairInventoryIntersection( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
												  components::inventory* inventoryComponent, const float inventorySlotScale, const float inventorySlotHalfScale );
		point2D<int> determineActualIntersectionSlot( components::transform* crosshairTransformComponent, components::transform* inventoryTransformComponent,
												 const float inventorySlotScale, const float inventorySlotHalfScale );

		bool          isInventoryOpened;
		int*          isItemDraged;
		bool*         isLeftMouseButtonReleased;
		bool          isLeftMouseButtonPressed;
		float         mouseOffsetX = 0;
		float         mouseOffsetY = 0;
		float         aspectRate   = 1.778;                   ///< Multiplier of current aspect rate. For full hd this must be 1920 / 1080
	};
} ///< namespace GLVM::ecs


#endif
