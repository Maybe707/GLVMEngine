#include "ArchetypeECS/ArchECS_Utils.hpp"

namespace GLVM::ecs::arch {
	bool matchesRequiredMask(const componentMask archetypeMask, const componentMask& systemMask) {
		return (archetypeMask & systemMask) == systemMask;
	}
};
