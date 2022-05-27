#include "Systems/CameraSystem.hpp"

namespace GLVM::ECS
{
    void CCameraSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ECS::CComponentManager::GetInstance();
        Core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ECS::GetInnerIDsContainer<ECS::CViewComponent>(*pComponent_Manager);
        unsigned int uiVector_View_Size = pEntity_Container_refView->GetSize();
        
        Shader_Program_->Use();
        Shader_Program_->SetUniformID();
        
        ECS::STransformComponent* Player_Transform_Component;
        for(int j = 0, iSize = uiVector_View_Size; j < iSize; ++j)
        {
            unsigned int uiEntity_refView = (*pEntity_Container_refView)[j];
            Player_Transform_Component = &(pComponent_Manager->GetComponent<ECS::STransformComponent>(uiEntity_refView));
            SetViewMatrix(*Player_Transform_Component, pComponent_Manager->GetComponent<ECS::CViewComponent>(uiEntity_refView));
        }
    }
    
    void CCameraSystem::SetViewMatrix(ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component)
    {
        Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mouse_Pointer_Position_.iOffset_X;
        fPitch = g_eEvent.mouse_Pointer_Position_.iOffset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mouse_Pointer_Position_.fPitch_ = fPitch;
        g_eEvent.mouse_Pointer_Position_.fYaw_ = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

        Vector<float, 3> front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        _view_Component.Front_Camera = Normalize(front);
 
        tView_Matrix = FPSview(_Player.tPosition,
                               _Player.tPosition + _view_Component.Front_Camera,
                               _view_Component.Up_Camera);

        // tView_Matrix = FPS_View_RH(_Player.tPosition,
        //                        fPitch,
        //                        fYaw);
        
        unsigned int uiTransform_View = pGLGet_Uniform_Location(Shader_Program_->iID, "aView_Matrix");
        pGLUniform_Matrix4fv(uiTransform_View, NUMBER_OF_MATRICES, GL_FALSE, &tView_Matrix[0][0]);

        SetProjectionMatrix();
    }

    void CCameraSystem::SetProjectionMatrix()
	{
                float f = 10, n = 0.1;
        float fov = 90;
        float S = 1 / std::tan((fov/2) * (PI / 180));
        tProjection_Matrix[0][0] = S;
        tProjection_Matrix[1][1] = S;
        tProjection_Matrix[2][2] = -(f / (f - n));
        tProjection_Matrix[2][3] = -1;
        tProjection_Matrix[3][2] = -((f * n) / (f - n));
        
		unsigned int uiTransformt = pGLGet_Uniform_Location(Shader_Program_->iID, "aProjection_Matrix");
		pGLUniform_Matrix4fv(uiTransformt, NUMBER_OF_MATRICES, GL_FALSE, &tProjection_Matrix[0][0]);
	}
}
