#ifndef PLAYER
#define PLAYER

#include "MatrixComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"

#define X_AXIS 12
#define Y_AXIS 13
#define MATRIX_RANGE 4

namespace GLVM::Core
{
	
	class CPlayer
	{
	// 	float aVertices_[30];
	// 	SVertexVector vertex_Vector_;
	// 	const char kSign_ = 'p';
	// 	CTexture Texture_;
	// 	Math::TCMatrix tModel_Matrix_;
	// public:
	// 	CPlayer(const CTexture& _Texture);
	// 	virtual SVertexVector& GetVertexVector() override;
	// 	virtual const char& GetSign() override;
	// 	void SetTexture(const CTexture& _Texture);
	// 	virtual CTexture& GetTexture() override;
	// 	void SetMatrix(Math::TCMatrix<MATRIX_RANGE>& _tModel_Matrix);
	// 	virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() override { return &tModel_Matrix_; }
	// 	void Move(double _dOffset, Core::CEvent _Event);
	// 	void SetVertex(float* _Vertex);
	};
	
}

#endif
