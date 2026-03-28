#ifndef COLLIDER_FLAGS_COMPONENT
#define COLLIDER_FLAGS_COMPONENT

namespace GLVM::ecs::components {
	struct colliderFlags {
		int flags : 4; ///< 0001 = groundCollision; 0010 = roofCollision; 0100 = wallCollision; 1000 = itemDrag
	};
}; // namespace GLVM::ecs::components

#endif
