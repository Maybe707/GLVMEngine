#ifndef VECTOR_CONTAINER
#define VECTOR_CONTAINER

namespace GLVM::Core
{

	template<class T>
	class TCVectorContainer
	{
		int iSize_ = 0;
		int iCapacity_ = 0;
		int iExpander_ = 10;
		T* aVector_Container_ = new T[iSize_];
	public:
		~TCVectorContainer()
		{
			delete [] aVector_Container_;
			aVector_Container_ = nullptr;
		}
		
		void Push(const T _Item)
		{
			if(iSize_ == iCapacity_)
			{
				T aTemp_Vector_Container[iCapacity_];
				if(iCapacity_ > 0)
				{
					for(int i = 0; i < iCapacity_; ++i)
						aTemp_Vector_Container[i] = aVector_Container_[i];
				}

				delete [] aVector_Container_;
				aVector_Container_ = nullptr;

				aVector_Container_ = new T[iCapacity_ + iExpander_];

				if(iCapacity_ > 0)
				{
					for(int j = 0; j < iCapacity_; ++j)
						aVector_Container_[j] = aTemp_Vector_Container[j];
				}

				iCapacity_ += iExpander_;
			
				aVector_Container_[iSize_] = _Item;
				++iSize_;
				return;
			}

			if(iSize_ < iCapacity_)
			{
				aVector_Container_[iSize_] = _Item;
				++iSize_;
				return;
			}
		}

		void RemoveItem(const T _Item)
		{
			if(iSize_ < 1)
				return;
		
			int iTemp_Index = 0;
			T aTemp_Vector_Container[iCapacity_];
			if(iCapacity_ > 0)
			{
				for(int i = 0; i < iCapacity_; ++i)
					aTemp_Vector_Container[i] = aVector_Container_[i];
			}
		
			for(int j = 0; j < iCapacity_; ++j)
			{
				if(_Item == aVector_Container_[j])
					continue;

				aVector_Container_[iTemp_Index] = aTemp_Vector_Container[j];
				++iTemp_Index;
			}
			--iSize_;
			aVector_Container_[iSize_] = 0;
		}

		T& GetItem(const T _Item)
		{
			for(int i = 0; i < iCapacity_; ++i)
				if(_Item == aVector_Container_[i])
					return aVector_Container_[i];
		}
	
		T& GetHead()
		{
			return aVector_Container_[iSize_-1];
		}

		T* GetVectorContainer()
		{
			return aVector_Container_;
		}

		int GetSize() { return iSize_; }
		int GetCapacity() { return iCapacity_; }
	};
	
}
    
#endif 
