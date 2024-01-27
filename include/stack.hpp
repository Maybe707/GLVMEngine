#ifndef CLASSIC_STACK
#define CLASSIC_STACK

#include <assert.h>

namespace GLVM::core
{    

	template <class T>
	class stack
	{
		unsigned int size_ = 0;
		unsigned int capacity = 0;
		unsigned int expander = 10;
	
		T* data = nullptr;
	public:
		void push(T element) {
			if (size_ == capacity) {
				capacity += capacity / 2 + expander;

				delete data;
				data = nullptr;
			
				data = new T[capacity];
			}

			data[size_] = element;
			++size_;
		}
	
		T pop() {
			assert(size_ > 0);
			unsigned int half_size = expander / 2;
			if ( size_ < half_size ) {
				T* temp = new T[half_size];
				for ( unsigned int i = 0; i < half_size; ++i )
					temp[i] = data[i];
				
				delete data;
				data = temp;

				temp = nullptr;
			}
			
			--size_;
			data[size_] = 999;
			return data[size_];
		}

		T& top() {
			assert(size_ > 0);
			unsigned int top = size_ - 1;
			return data[top];
		}

		bool contains(T element) {
			for ( unsigned int i = 0; i < size_; ++i ) {
				if ( data[i] == element )
					return true;
			}

			return false;
		}

		unsigned int size() { return size_; }
		bool empty() { return size_ == 0; }
		T operator[](unsigned int index) { return data[index]; }

		void remove(T element) {
			bool flag = false;
			for ( unsigned int i = 0; i < size_; ++i ) {
				if ( data[i] == element ) 
					flag = true;

				if ( flag && i == size_ - 1 )
					data[i] = 999;
				
				if ( flag ) 
					data[i] = data[i + 1];
			}

			if ( flag )
				size_ -= 1;
		}
	};

} // namespace GLVM::core
	
#endif

