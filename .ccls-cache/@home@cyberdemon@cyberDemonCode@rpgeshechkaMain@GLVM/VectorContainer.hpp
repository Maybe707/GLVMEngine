#ifndef VECTOR_CONTAINER
#define VECTOR_CONTAINER

#include "Constants.hpp"
#include "IContainer.hpp"

namespace GLVM::Core
{

	template<class T>
	class TCVectorContainer : public IContainer
	{
		unsigned int iSize_ = 0;
		unsigned int iCapacity_ = 0;
		unsigned int iExpander_ = 10;
		T* aVector_Container_ = new T[iSize_];
	public:
		~TCVectorContainer();
		void Push(const T _Item);
		void Insert(const T _Item, const unsigned int _Index);
		void RemoveItem(const T _Item);
		void RemoveFirstItem();
		T& GetItem(const T _Item);
		T& GetFirstItem();
		T& GetHead();
		T* GetVectorContainer();
		int GetSize();
		int GetCapacity();
		T& operator[](const unsigned int _iIndex);
	};

	template<class T>
	TCVectorContainer<T>::~TCVectorContainer()
	{
		delete [] aVector_Container_;
		aVector_Container_ = nullptr;
	}

	template<class T>
	void TCVectorContainer<T>::Push(const T _Item)
	{
		if(iSize_ == iCapacity_)
		{
			T* aTemp_Vector_Container = new T[iCapacity_ + iExpander_];
			for(unsigned int i = 0; i < iCapacity_; ++i)
				aTemp_Vector_Container[i] = aVector_Container_[i];

			delete [] aVector_Container_;
			aVector_Container_ = aTemp_Vector_Container;

			iCapacity_ += iExpander_;
			
			aVector_Container_[iSize_] = _Item;
			++iSize_;
			return;
		}

		aVector_Container_[iSize_] = _Item;
		++iSize_;
		return;
	}

	template<typename T>
	void TCVectorContainer<T>::Insert(const T _Item, const unsigned int _Index)
	{
		if(_Index >= iCapacity_)
		{
			unsigned int u_iTemp_Capacity = iCapacity_;
			iCapacity_ = (_Index + 1);
			T* aTemp_Vector_Container_ = new T[iCapacity_];

			for(unsigned int j = 0; j < u_iTemp_Capacity; ++j)
				aTemp_Vector_Container_[j] = aVector_Container_[j];

			delete [] aVector_Container_;
			aVector_Container_ = aTemp_Vector_Container_;
		}

		aVector_Container_[_Index] = _Item;
		++iSize_;
	}
	
	template<class T>
	void TCVectorContainer<T>::RemoveItem(const T _Item)
	{
		if(iSize_ < 1)
			return;

		bool bRemove_Flag = false;
		
		int iTemp_Index = 0;
		T aTemp_Vector_Container[iCapacity_];
		if(iCapacity_ > 0)
		{
			for(unsigned int i = 0; i < iCapacity_; ++i)
				aTemp_Vector_Container[i] = aVector_Container_[i];
		}
		
		for(unsigned int j = 0; j < iCapacity_; ++j)
		{
			if(_Item == aVector_Container_[j])
			{
				bRemove_Flag = true;
				continue;
			}

			aVector_Container_[iTemp_Index] = aTemp_Vector_Container[j];
			++iTemp_Index;
		}

		if(bRemove_Flag)
		{
			--iSize_;
			aVector_Container_[iSize_] = k_iNull;
		}
	}

	template<class T>
	void TCVectorContainer<T>::RemoveFirstItem()
	{
		if(iSize_ < 1)
			return;
		
		T aTemp_Vector_Container[iCapacity_];
		if(iCapacity_ > 0)
		{
			for(unsigned int i = 0; i < (iCapacity_-1); ++i)
				aTemp_Vector_Container[i] = aVector_Container_[i+1];
		}
		
		for(unsigned int j = 0; j < (iCapacity_-1); ++j)
			aVector_Container_[j] = aTemp_Vector_Container[j];
		--iSize_;
		aVector_Container_[iSize_] = k_iNull;
	}
	
	template<class T>
	T& TCVectorContainer<T>::GetItem(const T _Item)
	{
		for(int i = 0; i < iCapacity_; ++i)
			if(_Item == aVector_Container_[i])
				return aVector_Container_[i];
	}

	template<class T>
	T& TCVectorContainer<T>::GetFirstItem()
	{
		return aVector_Container_[k_iNull];
	}

	template<class T>
	T& TCVectorContainer<T>::GetHead()
	{
		return aVector_Container_[iSize_-1];
	}

	template<class T>
	T* TCVectorContainer<T>::GetVectorContainer()
	{
		return aVector_Container_;
	}

	template<typename T>
	int TCVectorContainer<T>::GetSize() { return iSize_; }
	template<typename T>
	int TCVectorContainer<T>::GetCapacity() { return iCapacity_; }
	template<typename T>
	T& TCVectorContainer<T>::operator[](const unsigned int _iIndex)
	{
		return aVector_Container_[_iIndex];
	}
}
    
#endif 
