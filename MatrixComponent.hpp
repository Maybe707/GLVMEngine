#ifndef MATRIX_COMPONENT
#define MATRIX_COMPONENT

#include "Event.hpp"
#include "TransformComponent.hpp"
#include "ConstVectorContainer.hpp"

#define LIMITER 1
#define HOMOGENEOUS_COORDINATE 1
#define OFFSET_X 0
#define OFFSET_Y 1

namespace GLVM::ECS
{
	class CMatrixComponent
	{
		inline static const unsigned int u_iRange = 4;
		float aMatrix_[u_iRange][u_iRange] {};
	public:
		CMatrixComponent(double _fScale = 0.1);

		float* GetMatrix();
		float (&Matrix())[u_iRange][u_iRange];
		void Offset(const STransformComponent& _transform_Component);
		void Move(double _dOffset, Core::CEvent _Event);
	};
}

#endif
