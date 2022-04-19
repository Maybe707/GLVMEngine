#ifndef EVENT
#define EVENT

//#include "Stack.hpp"

namespace GLVM::Core
{

	class CStack;
	
    /*! \enum Events
        \brief Realise event event types.
    */

    enum
    EEvents
    {
		eDEFAULT,
		eKEYRELEASE_A,
		eKEYRELEASE_D,
		eKEYRELEASE_S,
		eKEYRELEASE_W,
        eRENDER,
        eATACK,
        eSPAWN,
		eMOVE_UP,
		eMOVE_DOWN,
		eMOVE_LEFT,
		eMOVE_RIGHT,
		eGAME_LOOP_KILL,
    };

    /*! \class Event
        \brief Realise event game system.
    */
    
    class CEvent
    {
        EEvents eEvent_;
    
    public:
        CEvent();
        EEvents& GetEvent();
        void SetEvent(EEvents _eEvent);

		void SetLastEvent(CStack _Stack);
    };


}

#endif
