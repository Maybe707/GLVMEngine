#ifndef VIEW_COMPONENT
#define VIEW_COMPONENT

#include "VertexMath.hpp"

namespace GLVM::ecs::components
{
    class beholder
    {
    public:
        Vector<float, 3> forward{0.0f, 0.0, 0.0f};
        Vector<float, 3> up{0.0f, 0.0f, 0.0f};
        Vector<float, 3> right{0.0f, 0.0f, 0.0f};
		Vector<float, 3> Position{0.0f, 0.0f, 0.0f};
    };
}


#endif
