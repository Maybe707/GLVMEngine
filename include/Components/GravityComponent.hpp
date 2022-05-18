#ifndef GRAVITY_COMPONENT
#define GRAVITY_COMPONENT

#include "../VertexMath.hpp"

namespace GLVM::ECS
{
	class CGravityComponent
	{
    public:
		Vector<float, 3> tGravity_{0.0f, 0.0f, 0.0f};
	};
}

#endif
