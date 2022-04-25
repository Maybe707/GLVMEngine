#ifndef EVENT
#define EVENT

//#include "Stack.hpp"

namespace GLVM::Core
{

	class CStack;
	
    /*! \enum EEvents
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
        eMOUSE_POINTER_POSITION,
        eGAME_LOOP_KILL,
    };

    struct SMousePointerPosition
    {
        unsigned int u_iX;
        unsigned int u_iY;
    };
    
    /*! \class Event
        \brief Realise event game system.
    */

    class CEvent
    {
        EEvents eEvent_;
    
    public:
        SMousePointerPosition mouse_Pointer_Position_;
        
        CEvent();
        EEvents& GetEvent();
        void SetEvent(EEvents _eEvent);
		void SetLastEvent(CStack _Stack);
    };


}

#endif
