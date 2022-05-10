#ifndef VIEW_COMPONENT
#define VIEW_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ECS
{
    class CViewComponent
    {
    public:
        Vector<float, 3> Front_Camera{0.0f, 0.0, 0.0f};
        Vector<float, 3> Up_Camera{0.0f, 0.0f, 0.0f};
    };
}


#endif
