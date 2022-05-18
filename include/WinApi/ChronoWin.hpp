#ifndef CHRONOWIN
#define CHRONOWIN

#include <windows.h>
#include "IChrono.hpp"

namespace GLVM::Time
{    
	class CTimerWin : public IChrono
	{
		__int64 i64Freq_;
		__int64 i64Start_;
		__int64 i64Now_;
	
	public:
		CTimerWin();
	
		double InitFrequency();
		double Reset();
		double GetElapsed();
	};
}
	
#endif

