#ifndef SPATIAL_GRID_SYSTEM_HPP
#define SPATIAL_GRID_SYSTEM_HPP

#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs {

	class SpatialGridSystem : public ISystem {
		arch::Archetype* cachedArchetypes[32];
		uint32_t cachedArchetypesNumber = 0;
		
		struct SpatialGridComponentsView {
			components::transform* transforms = nullptr;
		} view;
		
		arch::componentMask requiredMask =
			(1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT);
		
		void Update() override;
	};
	
}; ///< namespace GLVM::core

#endif
