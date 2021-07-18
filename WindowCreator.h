#ifndef WINDOW_CREATOR
#define WINDOW_CREATOR

#include "IWindow.h"

namespace GLVM::Core
{    

    class CWindowCreator
    {
    public:
        IWindow* Create();
    };

}

#endif
