#ifndef ITIMER
#define ITIMER

namespace GLVM::Time
{
	class IChrono
	{
	public:
	    virtual ~IChrono() {}
		
		virtual double InitFrequency() = 0;
		virtual double Reset() = 0;
		virtual double GetElapsed() = 0;
	};
}

#endif
