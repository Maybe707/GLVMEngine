#ifndef GUI_SYSTEM
#define GUI_SYSTEM

#include "ComponentManager.hpp"
#include "Event.hpp"
#include "ISystem.hpp"
#include "ShaderProgram.hpp"
#include "Constants.hpp"
#include "VertexMath.hpp"
#include <GL/gl.h>

namespace GLVM::ECS
{
    class CGUISystem : public ISystem
    {
        GLuint iVbo_Crosshair_;
		GLuint iVao_Crosshair_;

    public:
        void Update() override;
        void LoadCrosshairTexture(const unsigned char* _kTexture_Data);

        Shader* _Shader_Program;
    };
}

#endif
