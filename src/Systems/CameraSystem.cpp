#include "Systems/CameraSystem.hpp"
#include "Components/ViewComponent.hpp"
#include "Systems/RenderSystem.hpp"
#include "VertexMath.hpp"

namespace GLVM::ecs
{
    void CCameraSystem::Update()
    {
		namespace cm = GLVM::ecs::components;
		
        ComponentManager* pComponent_Manager = GLVM::ecs::ComponentManager::GetInstance();
        core::vector<unsigned int>* pEntity_Container_refView =
			pComponent_Manager->GetEntityContainer<cm::beholder>();
        unsigned int uiVector_View_Size = pEntity_Container_refView->GetSize();
        
        // Shader_Program_->Use();
        // Shader_Program_->SetUniformID();
        
        cm::transform* Player_Transform_Component;
        for(int j = 0, iSize = uiVector_View_Size; j < iSize; ++j) {
            unsigned int uiEntity_refView = (*pEntity_Container_refView)[j];
            Player_Transform_Component = (pComponent_Manager->GetComponent<cm::transform>(uiEntity_refView));
            SetViewMatrix(*Player_Transform_Component, *pComponent_Manager->GetComponent<cm::beholder>(uiEntity_refView));
        }
    }
    
    void CCameraSystem::SetViewMatrix(components::transform& _Player, components::beholder& cameraComponent)
    {
        Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.1f;

        fYaw = g_eEvent.mousePointerPosition.offset_X;
        fPitch = g_eEvent.mousePointerPosition.offset_Y;
        fYaw *= kSensitivity;
        fPitch *= kSensitivity;

        g_eEvent.mousePointerPosition.pitch = fPitch;
        g_eEvent.mousePointerPosition.yaw = fYaw;
        
        if(fPitch > 89.0f)
            fPitch = 89.0f;
        if(fPitch < -89.0f)
            fPitch = -89.0f;

        Vector<float, 3> front;
        front[0] = std::cos(Radians(fYaw)) * std::cos(Radians(fPitch));
        front[1] = std::sin(Radians(fPitch));
        front[2] = std::sin(Radians(fYaw)) * std::cos(Radians(fPitch));
        cameraComponent.forward = Normalize(front);

        tView_Matrix = LookAtMain(_Player.tPosition,
								  _Player.tPosition + cameraComponent.forward,
								  cameraComponent.up);

		// tView_Matrix = LookAtMain(_Player.tPosition,
		// 						  vec3(0.0f, 0.0f, 0.0f),
		// 						  cameraComponent.up);

		
 		// cameraComponent.Position[0] = _Player.tPosition[0];
		// cameraComponent.Position[1] = _Player.tPosition[1];
		// cameraComponent.Position[2] = _Player.tPosition[2];

		Render_System_->SetViewMatrix(tView_Matrix);
        SetProjectionMatrix();
    }

    void CCameraSystem::SetProjectionMatrix()
	{
		tProjection_Matrix = Perspective(Radians(90.0f), (float)1920 / (float)1080, 1.0f, 100.0f);
        Render_System_->SetProjectionMatrix(tProjection_Matrix);
	}
}
