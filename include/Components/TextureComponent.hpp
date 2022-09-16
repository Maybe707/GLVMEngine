#ifndef TEXTURE_COMPONENT
#define TEXTURE_COMPONENT

#include <GL/gl.h>
#include "GLPointer.h"
#include "ShaderProgram.hpp"
#include <iostream>

namespace GLVM::ECS
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CTextureComponent
    {
	public:
        unsigned int id_;
        unsigned int vkInnerId_; ///< This field using to choose specific instance of texture image in Vulkan.
        
		// GLuint iTexture_;
		// unsigned int iWidth_;
		// unsigned int iHeight_;
		// unsigned int iNrChannels_;
        // unsigned int dat_length_;
		// const unsigned char* u_iData_;
	};
}
    
#endif
