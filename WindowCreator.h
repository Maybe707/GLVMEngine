#ifndef WINDOW_CREATOR
#define WINDOW_CREATOR

#include "IWindow.h"

/*! \class WindowCreator
    \brief Create vindow interface

    This class creates a window independent interface.
    Implemented by means of the factory method.
*/

namespace GLVM::Core
{    
    class CWindowCreator
    {
    public:
        IWindow* Create();
    };
}

#endif
