#ifndef ARCH_ECS_TYPES_HPP
#define ARCH_ECS_TYPES_HPP

#include <cstdint>

namespace GLVM::ecs::arch {
#define ENTITY_ID_BITS 32
#define GENERATION_BITS 32

	constexpr uint64_t entityBitsMask = (1ull << ENTITY_ID_BITS) - 1;
	
	typedef uint64_t entity;
	typedef uint32_t id;
	typedef uint32_t generation;
	typedef uint64_t componentMask;
};


#endif
