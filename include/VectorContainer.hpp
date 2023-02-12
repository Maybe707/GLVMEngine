#ifndef VECTOR_CONTAINER
#define VECTOR_CONTAINER

#include "Constants.hpp"
#include "IContainer.hpp"
#include <cstddef>
#include <iostream>
#include "VertexMath.hpp"
#include <assert.h>
#include "Iterator.hpp"

namespace GLVM::core
{
	template <class T>
	class TCVectorContainer;

	template <class T>
	class VectorIterator : public Iterator<T>
	{
		T* begin;
		T* end;
		
	public:
		VectorIterator(TCVectorContainer<T>& vector) {
			begin = vector.GetVectorContainer();
		    end   = vector.GetVectorContainer() + (vector.GetSize() - 1);
		}
	
		bool Next() override {
			if ( ValidStatus() ) {
				begin += 1;
				return true;
			} else
				return false;
		}

 		bool ValidStatus() override {
			return end >= begin;
		}
	
		T& Current() override {
			return *begin;
		}
		
		T& Last() override {
			return *end;
		}
	};
	
	template <class T>
	class TCVectorContainer;
	template <class T> using vector = GLVM::core::TCVectorContainer<T>;
	
	template<class T>
	class TCVectorContainer : public IContainer
	{
		unsigned int iSize_ = 0;
		unsigned int iCapacity_ = 0;
		unsigned int iExpander_ = 10;
		unsigned char* aVector_Container_ = nullptr;
	public:
        TCVectorContainer() {}
        TCVectorContainer(const TCVectorContainer<T>& _vector);
        ~TCVectorContainer();
		void Push(T _Item);
		void Pop();
		void Swap(T& firstElement, T& secondElement);
		VectorIterator<T> Find(T& element);
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
        void Print();
        TCVectorContainer& operator=(const TCVectorContainer<T>& _vector);
        bool operator==(const char* string_);
	};

    template <class T>
    bool TCVectorContainer<T>::operator==(const char* string_) {
		char tempSymbol = '2';
		unsigned int strSize = 0;
		while(tempSymbol != '\0') {
			tempSymbol = string_[strSize];
			++strSize;
		}
		
        for (unsigned int i = 0; i < iSize_; ++i) {
			T& element = *(T*)&aVector_Container_[i * sizeof(T)];
            if (element == string_[i])
                continue;
            else
                return false;
        }

        return true;
    }
    
    template <class T>
    TCVectorContainer<T>& TCVectorContainer<T>::operator=(const TCVectorContainer<T>& _vector)
    {
        if(this == &_vector)
            return *this;

		for(unsigned int j = 0; j < this->iSize_; ++j) {
			T& destinationElement = *(T*)&this->aVector_Container_[j * sizeof(T)];
			destinationElement.~T();
		}

		delete [] this->aVector_Container_;

		iCapacity_ = _vector.iCapacity_;
		iSize_     = _vector.iSize_;
		this->aVector_Container_ = new unsigned char[iCapacity_ * sizeof(T)];
		
        for(unsigned int i = 0; i < _vector.iSize_; ++i) {
			T& sourceElement = *(T*)&_vector.aVector_Container_[i * sizeof(T)];
			new (&aVector_Container_[i * sizeof(T)]) T(sourceElement);
		}

        return *this;
    }

    template <class T>
    TCVectorContainer<T>::TCVectorContainer(const TCVectorContainer<T>& _vector)
    {
		iSize_     = _vector.iSize_;
	    iCapacity_ = _vector.iSize_;
		this->aVector_Container_ = new unsigned char[iCapacity_ * sizeof(T)];
		
        for(unsigned int i = 0; i < _vector.iSize_; ++i) {
			T& sourceElement = *(T*)&_vector.aVector_Container_[i * sizeof(T)];
			new (&aVector_Container_[i * sizeof(T)]) T(sourceElement);
		}
    }
    
	template<class T>
	TCVectorContainer<T>::~TCVectorContainer()
	{
		for ( unsigned int i = 0; i < iSize_; ++i) {
			T& element = *(T*)&aVector_Container_[i * sizeof(T)];
			element.~T();
		}
		delete [] aVector_Container_;
		aVector_Container_ = nullptr;
	}

    /// Push element on top of the container.
    
	template<class T>
	void TCVectorContainer<T>::Push(T _Item)
	{
		if(iSize_ == iCapacity_)
			{
				unsigned char* aTemp_Vector_Container = new unsigned char[(iCapacity_ + iExpander_) * sizeof(T)];
				for(unsigned int i = 0; i < iCapacity_; ++i) {
					T& element = *(T*)&aVector_Container_[i * sizeof(T)];
					new (&aTemp_Vector_Container[i * sizeof(T)]) T(element);
					element.~T();
				}

				delete [] aVector_Container_;
				aVector_Container_ = aTemp_Vector_Container;

				iCapacity_ += iExpander_;
			
				new (&aVector_Container_[iSize_ * sizeof(T)]) T(_Item);
				++iSize_;
				return;
			}

		new (&aVector_Container_[iSize_ * sizeof(T)]) T(_Item);
		++iSize_;
	}

	template <class T>
	void TCVectorContainer<T>::Pop() {
		if ( iSize_ < 1 )
			return;

		T& element = *(T*)&aVector_Container_[(iSize_ - 1) * sizeof(T)];
		element = 0;                                                     ///< For debug purpoese only!!!
		element.~T();
		--iSize_;
	}

	template <class T>
	void TCVectorContainer<T>::Swap(T& firstElement, T& secondElement) {
		if ( iSize_ < 1)
			return;

		std::cout << "first element: " << firstElement << std::endl;
		std::cout << "second element: " << secondElement << std::endl;
		
		if ( &firstElement == &secondElement ) {
		    return;
		}
		
		T tempElement = firstElement;
		firstElement  = secondElement;
		secondElement  = tempElement;
	}

	template <class T>
	VectorIterator<T> TCVectorContainer<T>::Find(T& element) {
		VectorIterator<T> iterator(*this);
		if ( !iterator.ValidStatus() ) {
			std::cout << "Vector is empty. Retern iterator with pointer on end" << std::endl;
			return iterator;
		}

		do {
			if ( iterator.Current() == element )
				return iterator;
		} while ( iterator.Next() );

		std::cout << "Vector dont contain this element" << std::endl;
		return iterator;
	}
	
    /// Insert element into chosen cell.
    
	template<typename T>
	void TCVectorContainer<T>::Insert(T _Item, const unsigned int _Index)
	{
		if(_Index >= iCapacity_)
		{
			unsigned int u_iTemp_Capacity = _Index + iExpander_;
			unsigned char* aTemp_Vector_Container_ = new unsigned char[u_iTemp_Capacity * sizeof(T)];

			for(unsigned int j = 0; j < iCapacity_; ++j) {
				T& element = *(T*)&aVector_Container_[j * sizeof(T)];
				new (&aTemp_Vector_Container_[j * sizeof(T)]) T(element);
				element.~T();
			}

			new (&aTemp_Vector_Container_[_Index * sizeof(T)]) T(_Item);
			delete [] aVector_Container_;
			aVector_Container_ = aTemp_Vector_Container_;
			++iSize_;
			iCapacity_ = u_iTemp_Capacity;
			return;
		}

		new (&aVector_Container_[_Index * sizeof(T)]) T(_Item);
		++iSize_;
	}
	
	template<class T>
	void TCVectorContainer<T>::RemoveItem(const T _Item)
	{
		if(iSize_ < 1)
			return;

		// unsigned int i = 0;
		// for ( i = 0; i < iSize_; ++i) {
		// 	if ( _Item == aVector_Container_[i] )
		// 		break;
		// }

		// for ( ; i < iSize_ - 1; ++i )
		// 	aVector_Container_[i] = aVector_Container_[i + 1];

		// if ( i == iSize_ ) {
		// 	std::cout << "Vector dont contain this element" << std::endl;
		// 	return;
		// }

		// --iSize_;
		// aVector_Container_[iSize_] = k_iNull;

		for ( unsigned int i = 0; i < iSize_; ++i ) {
			if ( _Item == aVector_Container_[i] ) {
				aVector_Container_[i] = aVector_Container_[iSize_ - 1];
				aVector_Container_[iSize_ - 1] = k_iNull;
				--iSize_;
				return;
			}
		}

		std::cout << "Vector dont contain this element" << std::endl;
	}

    
	template<class T>
	void TCVectorContainer<T>::RemoveFirstItem()
	{
		if(iSize_ < 1)
			return;
		
		unsigned char* aTemp_Vector_Container = new unsigned char[iSize_ * sizeof(T)];
		for(unsigned int i = 0; i < iSize_; ++i) {
			T& element = *(T*)&aVector_Container_[i * sizeof(T)];
			new (&aTemp_Vector_Container[i * sizeof(T)]) T(element);
			element.~T();
		}

		delete [] this->aVector_Container_;

		--iSize_;				
		aVector_Container_ = new unsigned char[iSize_ * sizeof(T)];
        for(unsigned int i = 0; i < iSize_; ++i) {
			T& sourceElement = *(T*)&aTemp_Vector_Container[(i + 1) * sizeof(T)];
			new (&aVector_Container_[i * sizeof(T)]) T(sourceElement);
		}
	}
	
	template<class T>
	T& TCVectorContainer<T>::GetFirstItem()
	{
		return *(T*)&aVector_Container_[0];
	}

	template<class T>
	T& TCVectorContainer<T>::GetHead()
	{
		return *(T*)&aVector_Container_[(iSize_ - 1) * sizeof(T)];
	}

	template<class T>
	T* TCVectorContainer<T>::GetVectorContainer()
	{
		return (T*)aVector_Container_;
	}

	template<typename T>
	int TCVectorContainer<T>::GetSize() { return iSize_; }
	template<typename T>
	int TCVectorContainer<T>::GetCapacity() { return iCapacity_; }
	template<typename T>
	T& TCVectorContainer<T>::operator[](const unsigned int _iIndex)
	{
		return *(T*)&aVector_Container_[_iIndex * sizeof(T)];
	}

    template<class T>
    void TCVectorContainer<T>::Print()
    {
        for(unsigned int i = 0; i < iCapacity_; ++i)
            std::cout << (T)aVector_Container_[i * sizeof(T)] << std::endl;

        std::cout << "End of container" << std::endl;
    }
}
    
#endif 
