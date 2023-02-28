#ifndef RIGIDBODY_COMPONENT
#define RIGIDBODY_COMPONENT

namespace GLVM::ecs::components
{
	class rigidBody
	{
    public:
		float gravityTime;
        float fMass_;
        bool bGravity_;
	};
}

#endif
