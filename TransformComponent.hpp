#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ECS
{    
	struct STransformComponent
	{
        Vector<float, 3> tVertex{0.0f, 0.0f, 0.0f};
        float fRotate = 0.0f;
        float fScale = 1.0f;
	};
}

#endif
