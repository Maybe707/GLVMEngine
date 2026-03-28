#ifndef PHYSICS_COMPONENT
#define PHYSICS_COMPONENT

namespace GLVM::ecs::components {
	struct physics {
		float gravityAccumulator = 0.0f;
	};
}; // namespace GLVM::ecs::components

#endif
