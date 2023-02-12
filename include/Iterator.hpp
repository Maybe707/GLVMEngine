#ifndef I_ITERATOR
#define I_ITERATOR

namespace GLVM::core
{
	template <class T>
	class Iterator
	{
	public:
		virtual ~Iterator() {}

	    virtual bool Next()        = 0;
		virtual bool ValidStatus() = 0;
		virtual T& Current()       = 0;
		virtual T& Last()          = 0;
	};
}

#endif
