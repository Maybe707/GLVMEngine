#ifndef CHRONOX
#define CHRONOX

#include <ctime>
#include "IChrono.hpp"

namespace GLVM::Time
{
	class CTimerX : public IChrono
	{
		timespec start_;
		timespec now_;
		double lFrequency_;
		double lSeconds_;
		double lNanoseconds_;

	public:
		CTimerX();
	
		double InitFrequency();
		double Reset();
		double GetElapsed();
	};
}

#endif
