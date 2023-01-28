#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ECS
{    
	struct transform
	{
        Vector<float, 3> tPosition{0.0f, 0.0f, 0.0f};
        Vector<float, 3> tForward{0.0f, 0.0f, 0.0};
        Vector<float, 3> tRight{0.0f, 0.0f, 0.0};
        Vector<float, 3> tUp{0.0f, 0.0f, 0.0};
        float fRotate = 0.0f;
        float fScale = 1.0f;
        bool hud = false;
	};
}

#endif
