#include "Renderer.h"

namespace GLVM::Core
{
    CRenderer::CRenderer() {}
    
    void CRenderer::DrawSprite(CSprite& _Sprite)
    {
        pGLBind_Vertex_Array(*_Sprite.GetVao());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
