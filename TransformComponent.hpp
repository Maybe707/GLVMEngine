#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

namespace GLVM::ECS
{    
	struct STransformComponent
	{
		float fPos_X;
		float fPos_Y;
		float fPos_Z;
        float fRotate = 0.0f;
        float fScale = 1.0f;
	};
}

#endif
