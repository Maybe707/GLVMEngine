#ifndef RIGIDBODY_COMPONENT
#define RIGIDBODY_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ecs::components
{
	class rigidBody
	{
    public:
		float gravityTime;
        float fMass_;
        bool bGravity_;
		vec3 jump{ 0.0f, 0.0f, 0.0f };
		float jumpAccumulator = 0.0f;
	};
}

#endif
