#include "Systems/RenderSystem.hpp"
#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Components/TextureComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "GLPointer.h"
#include "VectorContainer.hpp"
#include "Components/VertexComponent.hpp"
#include "VertexMath.hpp"
#include "Components/ViewComponent.hpp"
#include <GL/gl.h>
#include <cmath>
#include "Globals.hpp"

namespace GLVM::ECS
{
    CRenderSystem::CRenderSystem()
	{	

	}

	CRenderSystem::~CRenderSystem()
	{
		pGLDelete_Vertex_Arrays(NUMBER_OF_CREATING_VAO_OBJECT_1, &iVao_);
        pGLDelete_Buffers(NUMBER_OF_CREATING_VBO_OBJECT_1, &iVbo_);
	}
    
	void CRenderSystem::Update()
	{

	}

	void CRenderSystem::SetModelMatrix(Shader* _Shader_Program, ECS::STransformComponent& _transform_Component)
	{
        
	}
}
