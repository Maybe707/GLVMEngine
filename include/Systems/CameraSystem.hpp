#ifndef CAMERA_SYSTEM
#define CAMERA_SYSTEM

#include "ISystem.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/ViewComponent.hpp"
#include "ShaderProgram.hpp"
#include "Globals.hpp"
#include "Systems/RenderSystem.hpp"
#include "VertexMath.hpp"

namespace GLVM::ECS
{
    class CCameraSystem : public ISystem
    {
    public:
        CRenderSystem* Render_System_;
        Shader* Shader_Program_;
        Matrix<float, 4> tProjection_Matrix{1.0f};

        ///< Mouse parameters.
        float fYaw = -90.0f;
        float fPitch = 0.0f;
        float fLast_X = 1920.0f / 2.0f;
        float fLast_Y = 1080.0f / 2.0f;
        bool bFirst_Mouse = true;

        void Update() override;
        void SetViewMatrix(ECS::STransformComponent& _Player, ECS::CViewComponent& _view_Component);
        void SetProjectionMatrix();
    };
}

#endif
