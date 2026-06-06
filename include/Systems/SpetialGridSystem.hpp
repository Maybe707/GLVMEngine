#ifndef SPETIAL_GRID_SYSTEM_HPP
#define SPETIAL_GRID_SYSTEM_HPP

#include "ISystem.hpp"
#include "VertexMath.hpp"
#include "ArchetypeECS/ArchECS_World.hpp"
#include "ArchetypeECS/ArchetypeInterface.hpp"

namespace GLVM::ecs {

	class SpecialGridSystem : public ISystem {
		arch::Archetype* cachedArchetypes[32];
		uint32_t cachedArchetypesNumber = 0;
		
		struct SpecialGridComponentsView {
			components::transform* transforms = nullptr;
		} view;
		
		arch::componentMask requiredMask =
			(1ul << arch::ComponentsIndices::TRANSFORM_COMPONENT);
		
		void Update() override;
	};
	
}; ///< namespace GLVM::core

#endif
