#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ecs::components
{    
	struct transform
	{
        vec3 tPosition{ 0.0f, 0.0f, 0.0f };
        vec3 tForward{ 0.0f, 0.0f, 0.0f };
        vec3 tRight{ 0.0f, 0.0f, 0.0f };
        vec3 tUp{ 0.0f, 0.0f, 0.0 };
        float rotate = 0.0f;
        float fScale = 1.0f;
        bool hud = false;
		float GravityAccumulator = 0.0f;
	};
}

#endif
