#ifndef SPETIAL_GRID_SYSTEM_HPP
#define SPETIAL_GRID_SYSTEM_HPP

#include "ISystem.hpp"
#include "VertexMath.hpp"

namespace GLVM::core {

	class SpecialGridUpdateSystem : public ecs::ISystem {
		void Update() override;
	};
	
}; ///< namespace GLVM::core

#endif
