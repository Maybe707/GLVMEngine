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
	class vector;

	template <class T>
	class VectorIterator : public Iterator<T>
	{
		T* begin;
		T* end;
		
	public:
		VectorIterator(vector<T>& vector) {
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
	
	template<class T>
	class vector : public IContainer
	{
		unsigned int size = 0;
		unsigned int capacity = 0;
		unsigned int expander = 10;
		unsigned char* rowInnerData = nullptr;
	public:
        vector() {}
        vector(const vector<T>& _vector);
        ~vector();
		void Push(T _Item);
		void Pop();
		void Swap(T& firstElement, T& secondElement);
		VectorIterator<T> Find(T& element);
		void Resize(const unsigned int _Index);
		void Remove(unsigned int index);
		void RemoveFirstItem();
		T& GetItem(const T _Item);
		T& GetFirstItem();
		T& GetHead();
		T* GetVectorContainer();
		unsigned int GetSize();
		int GetCapacity();
		T& operator[](const unsigned int _iIndex);
		void clear();
        void Print();
        vector& operator=(const vector<T>& _vector);
        bool operator==(const char* string_);
	};

    template <class T>
    bool vector<T>::operator==(const char* string_) {
		char tempSymbol = '2';
		unsigned int strSize = 0;
		while(tempSymbol != '\0') {
			tempSymbol = string_[strSize];
			++strSize;
		}
		
        for (unsigned int i = 0; i < size; ++i) {
			T& element = *(T*)&rowInnerData[i * sizeof(T)];
            if (element == string_[i])
                continue;
            else
                return false;
        }

        return true;
    }
    
    template <class T>
    vector<T>& vector<T>::operator=(const vector<T>& _vector)
    {
        if(this == &_vector)
            return *this;

		for(unsigned int j = 0; j < this->size; ++j) {
			T& destinationElement = *(T*)&this->rowInnerData[j * sizeof(T)];
			destinationElement.~T();
		}

		delete [] this->rowInnerData;

		capacity = _vector.capacity;
		size     = _vector.size;
		this->rowInnerData = new unsigned char[capacity * sizeof(T)];
		
        for(unsigned int i = 0; i < _vector.size; ++i) {
			T& sourceElement = *(T*)&_vector.rowInnerData[i * sizeof(T)];
			new (&rowInnerData[i * sizeof(T)]) T(sourceElement);
		}

        return *this;
    }

    template <class T>
    vector<T>::vector(const vector<T>& _vector)
    {
		size     = _vector.size;
	    capacity = _vector.size;
		this->rowInnerData = new unsigned char[capacity * sizeof(T)];
		
        for(unsigned int i = 0; i < _vector.size; ++i) {
			T& sourceElement = *(T*)&_vector.rowInnerData[i * sizeof(T)];
			new (&rowInnerData[i * sizeof(T)]) T(sourceElement);
		}
    }
    
	template<class T>
	vector<T>::~vector()
	{
		for ( unsigned int i = 0; i < size; ++i) {
			T& element = *(T*)&rowInnerData[i * sizeof(T)];
			element.~T();
		}
		delete [] rowInnerData;
		rowInnerData = nullptr;
	}

    /// Push element on top of the container.
    
	template<class T>
	void vector<T>::Push(T _Item)
	{
		if(size == capacity)
			{
				unsigned char* aTemp_Vector_Container = new unsigned char[(capacity + expander) * sizeof(T)];
				for(unsigned int i = 0; i < capacity; ++i) {
					T& element = *(T*)&rowInnerData[i * sizeof(T)];
					new (&aTemp_Vector_Container[i * sizeof(T)]) T(element);
					element.~T();
				}

				delete [] rowInnerData;
				rowInnerData = aTemp_Vector_Container;

				capacity += expander;
			
				new (&rowInnerData[size * sizeof(T)]) T(_Item);
				++size;
				return;
			}

		new (&rowInnerData[size * sizeof(T)]) T(_Item);
		++size;
	}

	template <class T>
	void vector<T>::Pop() {
		if ( size < 1 )
			return;

		T& element = *(T*)&rowInnerData[(size - 1) * sizeof(T)];
		// if ( typeid(T).name() == typeid(unsigned int).name() ) {
		// 	element = 0;                                                     ///< For debug purpouses only!!!
		// }
		element.~T();
		--size;
	}

	template <class T>
	void vector<T>::Swap(T& firstElement, T& secondElement) {
		if ( size < 1)
			return;

		if ( &firstElement == &secondElement ) {
		    return;
		}
		
		T tempElement = firstElement;
		firstElement  = secondElement;
		secondElement  = tempElement;
	}

	template <class T>
	VectorIterator<T> vector<T>::Find(T& element) {
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
	void vector<T>::Resize(const unsigned int index)
	{
		if ( index < size ) {
			for(unsigned int j = index; j < size; ++j) {
				(*(T*)&rowInnerData[j * sizeof(T)]).~T();
			}

			size = index;
		} else if( index > size ) {
			if ( index > capacity ) {
				unsigned char* aTemp_Vector_Container_ = new unsigned char[index * sizeof(T)];

				for(unsigned int j = 0; j < size; ++j) {
					T& element = *(T*)&rowInnerData[j * sizeof(T)];
					new (&aTemp_Vector_Container_[j * sizeof(T)]) T(element);
					element.~T();
				}

				delete [] rowInnerData;
				rowInnerData = aTemp_Vector_Container_;
				capacity = index;
			}

			for ( unsigned int i = size; i < index; ++i) {
				new (&rowInnerData[i * sizeof(T)]) T{};
			}

			size = index;
		}
	}
	
 	template<class T>
	void vector<T>::Remove(unsigned int index)
	{
		if(size < 1)
			return;

		for(unsigned int j = index; j < size - 1; ++j) {
			T& element = *(T*)&rowInnerData[(j + 1) * sizeof(T)];
			T& previousElement = *(T*)&rowInnerData[j * sizeof(T)];
			previousElement.~T();
			new (&rowInnerData[j * sizeof(T)]) T(element);
		}

	    --size;
	}
    
	template<class T>
	void vector<T>::RemoveFirstItem()
	{
		if(size < 1)
			return;
		
		unsigned char* aTemp_Vector_Container = new unsigned char[size * sizeof(T)];
		for(unsigned int i = 0; i < size; ++i) {
			T& element = *(T*)&rowInnerData[i * sizeof(T)];
			new (&aTemp_Vector_Container[i * sizeof(T)]) T(element);
			element.~T();
		}

		delete [] this->rowInnerData;

		--size;				
		rowInnerData = new unsigned char[size * sizeof(T)];
        for(unsigned int i = 0; i < size; ++i) {
			T& sourceElement = *(T*)&aTemp_Vector_Container[(i + 1) * sizeof(T)];
			new (&rowInnerData[i * sizeof(T)]) T(sourceElement);
		}
	}
	
	template<class T>
	T& vector<T>::GetFirstItem()
	{
		return *(T*)&rowInnerData[0];
	}

	template<class T>
	T& vector<T>::GetHead()
	{
		return *(T*)&rowInnerData[(size - 1) * sizeof(T)];
	}

	template<class T>
	T* vector<T>::GetVectorContainer()
	{
		return (T*)rowInnerData;
	}

	template<typename T>
	unsigned int vector<T>::GetSize() { return size; }
	template<typename T>
	int vector<T>::GetCapacity() { return capacity; }
	template<typename T>
	T& vector<T>::operator[](const unsigned int _iIndex)
	{
		return *(T*)&rowInnerData[_iIndex * sizeof(T)];
	}

	template<typename T>
	void vector<T>::clear() {
		if(size < 1)
			return;
		
		for(unsigned int i = 0; i < size; ++i) {
			T& element = *(T*)&rowInnerData[i * sizeof(T)];
			element.~T();
		}

		unsigned int sizeOfType = sizeof(T);
		for (unsigned int j = 0; j < capacity * sizeOfType; ++j) {
			*(unsigned char*)&rowInnerData[j] = 0;
		}
		
		// delete [] this->rowInnerData;
		// this->rowInnerData = nullptr;

		size     = 0;
		expander = 0;
	}
	
    template<class T>
    void vector<T>::Print()
    {
        for(unsigned int i = 0; i < capacity; ++i)
            std::cout << *(T*)&rowInnerData[i * sizeof(T)] << std::endl;

        std::cout << "End of container" << std::endl;
    }
}
    
#endif 
