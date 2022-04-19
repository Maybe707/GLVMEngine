#ifndef STACK
#define STACK

#include "Event.hpp"

namespace GLVM::Core
{
	class CStack
	{
		int iHead_ = 0;
		static const int iStack_Range_ = 4;
		EEvents aStack_[iStack_Range_] = {};
	public:
		void Push(const EEvents& _Event)
		{

			for(int i = 0; i < iHead_; ++i)
			{
				if(aStack_[iHead_ - 1] == _Event)
					return;
			}

			if(iHead_ == iStack_Range_)
				return;
		
			aStack_[iHead_] = _Event;
				
			++iHead_;
		}

		EEvents& Pop()
		{
			return aStack_[iHead_-1];
		}

		void Remove(const EEvents& _Event)
		{
			EEvents aTemp_Stack[iStack_Range_] = {};
			int n = 0;
		
			for(int j = 0; j < iStack_Range_; ++j)
				aTemp_Stack[j] = aStack_[j];

			for(int i = 0; i < iHead_; ++i)
			{
				if(_Event == aTemp_Stack[i])
					continue;

				aStack_[n] = aTemp_Stack[i];
				++n;
			}

			--iHead_;
			aStack_[iHead_] = EEvents::eDEFAULT;
		}

		void ControlInput(CEvent& _eEvent)
		{ 
			switch(_eEvent.GetEvent())
			{
			case eGAME_LOOP_KILL:
				Push(eGAME_LOOP_KILL);
				break;
			case eKEYRELEASE_A:
				Remove(eMOVE_LEFT);
				break;
			case eKEYRELEASE_D:
				Remove(eMOVE_RIGHT);
				break;
			case eKEYRELEASE_S:
				Remove(eMOVE_DOWN); 
				break;
			case eKEYRELEASE_W:
				Remove(eMOVE_UP); 
				break;
			case eMOVE_LEFT:
				Push(eMOVE_LEFT);
				break;
			case eMOVE_RIGHT:
				Push(eMOVE_RIGHT);
				break;
			case eMOVE_DOWN:
				Push(eMOVE_DOWN);
				break;
			case eMOVE_UP:
				Push(eMOVE_UP);
				break;
			default:
				break;
			}
		}
	};
	
}
#endif
