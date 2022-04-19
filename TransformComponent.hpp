#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

namespace GLVM::ECS
{    
	struct STransformComponent
	{
		float fPos_X;
		float fPos_Y;
		float fPos_Z;
        float fRotate = 0;
	};
}

#endif
