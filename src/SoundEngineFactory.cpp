#include "SoundEngineFactory.hpp"

#ifdef __linux__
#include "SoundEngineAlsa.hpp"
#endif

#ifdef _WIN32
#include "SoundEngineWaveform.hpp"
#endif

namespace GLVM::core::Sound
{
    ISoundEngine* CSoundEngineFactory::CreateSoundEngine()
    {
#ifdef __linux__
        return new CSoundEngineAlsa;
#endif

#ifdef _WIN32
        return new CSoundEngineWaveform;
#endif
    }
}
