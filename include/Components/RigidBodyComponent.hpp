#ifndef RIGIDBODY_COMPONENT
#define RIGIDBODY_COMPONENT

namespace GLVM::ECS
{
	class CRigidBodyComponent
	{
    public:
        float fMass_;
        bool bGravity_;
	};
}

#endif
