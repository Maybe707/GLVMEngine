#ifndef EVENT
#define EVENT

namespace GLVM::Core
{
    
    /*! \enum Events
        \brief Realise event event types.
    */

    enum EEvents
    {
		eDEFAULT,
		eKEYRELEASE_A,
		eKEYRELEASE_D,
		eKEYRELEASE_S,
		eKEYRELEASE_W,
        eRENDER,
        eATACK,
        eSPAWN,
        eEXIT,
		eMOVE_UP,
		eMOVE_DOWN,
		eMOVE_LEFT,
		eMOVE_RIGHT,
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
    };
}

#endif
