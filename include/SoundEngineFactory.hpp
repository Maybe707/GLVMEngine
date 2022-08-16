#ifndef SOUND_ENGINE_FACTORY
#define SOUND_ENGINE_FACTORY

#include "ISoundEngine.hpp"

namespace GLVM::Core::Sound
{    
/*!
  \brief Create sound engine enterface.

  This class creates a sound engine independent interface.
  Implemented by means of the factory method.
*/

    class CSoundEngineFactory
    {
    public:
        ISoundEngine* CreateSoundEngine();
    };
    
}
#endif
