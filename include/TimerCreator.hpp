#ifndef TIMER_CREATOR
#define TIMER_CREATOR

#include "IChrono.hpp"

/*! \class TimerCreator
    \brief Create timer interface

	This class creates a timer independent interface.
	Implemented by means of the factory method.
*/

namespace GLVM::Time
{
	class CTimerCreator
	{
	public:
        ~CTimerCreator() {}
        
		IChrono* Create();
	};
}

#endif
