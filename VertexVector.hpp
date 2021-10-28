#ifndef VERTEX_VECTOR
#define VERTEX_VECTOR

#include "ConstVectorContainer.hpp"

namespace GLVM::Core
{    
	struct SVertexVector
	{
		static TCConstVectorContainer<SVertexVector*> s_tVertexVector_Componentes_Array;
		float fAxis_X;
		float fAxis_Y;
		float fAxis_Z;
		SVertexVector(const unsigned int _Entity_ID)
		{
			s_tVertexVector_Componentes_Array.Push(this, _Entity_ID);
		}
	};
}

#endif
