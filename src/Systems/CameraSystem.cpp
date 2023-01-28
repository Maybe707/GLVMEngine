#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "VertexMath.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLVM::ecs
{
    void CCameraSystem::Update()
    {
        CComponentManager* pComponent_Manager = GLVM::ecs::CComponentManager::GetInstance();
        core::TCVectorContainer<unsigned int>* pEntity_Container_refView =
            ecs::GetInnerIDsContainer<ecs::beholder>(*pComponent_Manager);
        unsigned int uiVector_View_Size = pEntity_Container_refView->GetSize();
        
        // Shader_Program_->Use();
        // Shader_Program_->SetUniformID();
        
        ecs::transform* Player_Transform_Component;
        for(int j = 0, iSize = uiVector_View_Size; j < iSize; ++j)
        {
            unsigned int uiEntity_refView = (*pEntity_Container_refView)[j];
            Player_Transform_Component = &(pComponent_Manager->GetComponent<ecs::transform>(uiEntity_refView));
            SetViewMatrix(*Player_Transform_Component, pComponent_Manager->GetComponent<ecs::beholder>(uiEntity_refView));
        }
    }
    
    void CCameraSystem::SetViewMatrix(ecs::transform& _Player, ecs::beholder& _view_Component)
    {
        Matrix<float, 4> tView_Matrix(1.0f);
        const float kSensitivity = 0.05f;

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

        // tView_Matrix = LookAtMain(_Player.tPosition,
		// 						  _Player.tPosition + _view_Component.Front_Camera,
		// 						  _view_Component.Up_Camera);

		tView_Matrix = LookAtMain(_Player.tPosition,
								  vec3(0.0f, 0.0f, 0.0f),
								  _view_Component.Up_Camera);

		
 		_view_Component.Position[0] = _Player.tPosition[0];
		_view_Component.Position[1] = _Player.tPosition[1];
		_view_Component.Position[2] = _Player.tPosition[2];

		Render_System_->SetViewMatrix(tView_Matrix);
        SetProjectionMatrix();
    }

    void CCameraSystem::SetProjectionMatrix()
	{
		tProjection_Matrix = Perspective(Radians(90.0f), (float)1024 / (float)1024, 1.0f, 25.0f);
        Render_System_->SetProjectionMatrix(tProjection_Matrix);
	}
}
