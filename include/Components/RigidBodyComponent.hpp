#ifndef RIGIDBODY_COMPONENT
#define RIGIDBODY_COMPONENT

namespace GLVM::ecs::components
{
	class rigidBody
	{
    public:
        float fMass_;
        bool bGravity_;
	};
}

#endif
