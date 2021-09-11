#ifndef EVENT
#define EVENT

namespace GLVM::Core
{
    
    /*! \enum Events
        \brief Realise event event types.
    */

    enum EEvents
    {
        eRENDER,
        eATACK,
        eSPAWN,
        eEXIT,
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
