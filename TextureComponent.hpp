#ifndef TEXTURE_COMPONENT
#define TEXTURE_COMPONENT

#include <GL/gl.h>
#include "GLPointer.h"
#include "ShaderProgram.hpp"
#include <iostream>
#include "ConstVectorContainer.hpp"

namespace GLVM::ECS
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CTextureComponent
    {
	public:
		GLuint iTexture_;
		unsigned int iWidth_;
		unsigned int iHeight_;
		unsigned int iNrChannels_;
		const unsigned char* u_iData_;
	};
}
    
#endif
