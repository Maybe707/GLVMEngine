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
        eKEYRELEASE_JUMP,
        eGRAVITY_COLLISION_FLAG,
        eRENDER,
        eATACK,
        eSPAWN,
        eJUMP,
		eMOVE_UP,
		eMOVE_DOWN,
		eMOVE_LEFT,
		eMOVE_RIGHT,
        eMOVE_DIAGONAL,
        eMOUSE_POINTER_POSITION,
        eGAME_LOOP_KILL,
        eEmpty,
    };

    struct SMousePointerPosition
    {
        int iPosition_X;
        int iPosition_Y;
        int iOffset_X = 0;
        int iOffset_Y = 0;
        float fPitch_;
        float fYaw_;
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
