#ifndef CAMERA_SYSTEM
#define CAMERA_SYSTEM

#include "RenderSystem.hpp"

namespace GLVM::ECS
{
    class CCameraSystem : public 
    {
    public:
        void SetViewMatrix(Shader* _Shader_Program, const ECS::STransformComponent& _transform_Component)
        {
            aMatrix_View_[0][0] = 1.0;
            aMatrix_View_[1][1] = 1.0;
            aMatrix_View_[2][2] = 1.0;
            aMatrix_View_[3][3] = 1.0;
            aMatrix_View_[u_iRange-LIMITER][0] = -_transform_Component.fPos_X;
            aMatrix_View_[u_iRange-LIMITER][1] = -_transform_Component.fPos_Y;
//        aMatrix_View_[u_iRange-LIMITER][2] = -_transform_Component.fPos_Z;
            unsigned int uiTransform_View = pGLGet_Uniform_Location(_Shader_Program->iID, "aView_Matrix");
            pGLUniform_Matrix4fv(uiTransform_View, NUMBER_OF_MATRICES, GL_FALSE, *aMatrix_View_);
        }
    };
}

#endif
