#ifndef WINDOW_CREATOR
#define WINDOW_CREATOR

#include "IWindow.hpp"

namespace GLVM::core
{
    
/*!
  \brief Create vindow interface.

  This class creates a window independent interface.
  Implemented by means of the factory method.
*/
    
    class CWindowCreator
    {
    public:
        IWindow* Create();
    };
}

#endif
