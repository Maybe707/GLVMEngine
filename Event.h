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
        bool bReturn_Flag_;
    
    public:
        CEvent();
        EEvents& GetEvent();
        bool& GetReturnFlag();
        void SetEvent(EEvents _eEvent);

        ///< Function that sets flag using for exit from game.
        
        void SetReturnFlag(bool _bReturn_Flag);
    };
}

#endif
