#ifndef STACK
#define STACK

#include "Event.hpp"
#include <iostream>

namespace GLVM::Core
{
	class CStack
{
	int iHead_ = 0;
	static const int iStack_Range_ = 4;
	EEvents aStack[iStack_Range_] = {};
public:
	void Push(const EEvents& _Event)
	{

		for(int i = 0; i < iHead_; ++i)
		{
			if(aStack[iHead_ - 1] == _Event)
				return;
		}

		if(iHead_ == 4)
		{
			return;
		}
		aStack[iHead_] = _Event;
				
		++iHead_;
	}

	EEvents& Pop()
	{
		return aStack[iHead_-1];
	}

	void Remove(const EEvents& _Event)
	{
		EEvents aTemp_Stack[iStack_Range_] = {};
		int n = 0;
		
		for(int j = 0; j < iStack_Range_; ++j)
			aTemp_Stack[j] = aStack[j];

		for(int i = 0; i < iHead_; ++i)
		{
			if(_Event == aTemp_Stack[i])
				continue;

			aStack[n] = aTemp_Stack[i];
			++n;
		}

		--iHead_;
		aStack[iHead_] = EEvents::eDEFAULT;
	}
	
	// void Remove(const EEvents& _Event)
	// {
	// 	EEvents aTemp_Stack[iStack_Range_] = {};
			
	// 	for(int j = 0; j < iStack_Range_; ++j)
	// 		aTemp_Stack[j] = aStack[j];

	// 	bool bFlag = false;
		
	// 	for(int i = 0; i < iHead_; ++i)
	// 	{
	// 		if((i+1) == iHead_)
	// 			break;

	// 		if(_Event == aTemp_Stack[i])
	// 			bFlag = true;

	// 		if(bFlag)
	// 		{
	// 			aStack[i] = aTemp_Stack[i+1];
	// 			continue;
	// 		}

	// 		aStack[i] = aTemp_Stack[i];
	// 	}

	// 	--iHead_;
	// 	aStack[iHead_] = EEvents::eDEFAULT;
	// }

	void Show()
	{
		for(int i = 0; i < iStack_Range_; ++i)
			std::cout << aStack[i] << std::endl;
		std::cout << "Stack" << std::endl;
	}
};
}
#endif
