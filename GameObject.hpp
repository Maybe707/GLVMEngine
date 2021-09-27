#ifndef GAME_OBJECT
#define GAME_OBJECT

#include "Matrix.hpp"
#include "Texture.hpp"

#define MATRIX_RANGE 4

namespace GLVM::Core
{
	
	class IGameObject
	{
	public:
		virtual ~IGameObject() {}

		virtual CTexture& GetTexture() = 0;
		virtual Math::TCMatrix<MATRIX_RANGE>* GetMatrix() = 0;
	};
		
}

#endif
