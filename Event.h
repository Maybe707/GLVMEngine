#ifndef EVENT
#define EVENT

#include <X11/Xlib.h>

namespace GLVM::Core
{    

class CEvent
{
     enum EEvents
     {
         eRENDER,
         eATACK,
         eSPAWN,
     };

     EEvents eEvent;

public:

};

}

#endif
