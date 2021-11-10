#ifndef CONST_VECTOR_CONTAINER
#define CONST_VECTOR_CONTAINER

#include "Constants.hpp"
#include "IContainer.hpp"
#include "ExtraFunctions.hpp"

namespace GLVM::Core
{

	template<class T>
	class TCConstVectorContainer : public IContainer
	{
		unsigned int iSize_ = 0;
		unsigned int iCapacity_ = 0;
		unsigned int iExpander_ = 10;
		T* aConst_Vector_Container_ = new T[iSize_];
	public:
		~TCConstVectorContainer();
		void Remove(const unsigned int _Index);
		void Push(const T _Item, const unsigned int _Index);
		void Push(const unsigned int _Index);
		T& operator[](const unsigned int _Index);
		unsigned int GetSize();
		unsigned int GetCapacity();
		T* GetContainer();
	};
	
	template<typename T>
	TCConstVectorContainer<T>::~TCConstVectorContainer()
	{
		delete [] aConst_Vector_Container_;
		aConst_Vector_Container_ = nullptr;
	}

	// template<typename T>
    // void TCConstVectorContainer<T>::Remove(const unsigned int _Index)
	// {
	// 	aConst_Vector_Container_[_Index] = nullptr;
	// }

	template<typename T>
	void TCConstVectorContainer<T>::Push(const T _Item, const unsigned int _Index)
	{
		if(_Index >= iCapacity_)
		{
			unsigned int u_iTemp_Capacity = iCapacity_;
			iCapacity_ = (_Index + 1);
			T* aTemp_Const_Vector_Container_ = new T[iCapacity_];

			for(unsigned int j = 0; j < u_iTemp_Capacity; ++j)
				aTemp_Const_Vector_Container_[j] = aConst_Vector_Container_[j];

			delete [] aConst_Vector_Container_;
			aConst_Vector_Container_ = aTemp_Const_Vector_Container_;
		}

		aConst_Vector_Container_[_Index] = _Item;
		++iSize_;
	}

	template<>
	inline void TCConstVectorContainer<unsigned int>::Push(const unsigned int _Index)
	{
		if(_Index >= iCapacity_)
		{
			unsigned int u_iTemp_Capacity = iCapacity_;
			iCapacity_ = (_Index + 1);
			unsigned int* aTemp_Const_Vector_Container_ =
				new unsigned int[iCapacity_];

			for(unsigned int j = 0; j < u_iTemp_Capacity; ++j)
				aTemp_Const_Vector_Container_[j] = aConst_Vector_Container_[j];

			delete [] aConst_Vector_Container_;
			aConst_Vector_Container_ = aTemp_Const_Vector_Container_;
		}

		aConst_Vector_Container_[_Index] = _Index;
		++iSize_;
	}
	
	template<typename T>
	T& TCConstVectorContainer<T>::operator[](const unsigned int _Index)
	{
		return aConst_Vector_Container_[_Index];
	}
	
	template<typename T>
	unsigned int TCConstVectorContainer<T>::GetSize() { return iSize_; }
	template<typename T>
	unsigned int TCConstVectorContainer<T>::GetCapacity() { return iCapacity_; }

	template <typename T>
	T* TCConstVectorContainer<T>::GetContainer()
	{
		return aConst_Vector_Container_;
	}
}
    
#endif 
