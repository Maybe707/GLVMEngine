#ifndef TEXTURE
#define TEXTURE

#include <GL/gl.h>
#include "GLPointer.h"
#include "ShaderProgram.hpp"
//#include "stb_image.h"
#include <iostream>

namespace GLVM::Core
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CTexture
    {
		GLuint iTexture_;
		int iWidth_;
		int iHeight_;
		int iNrChannels_;
		const unsigned char* uiData_;
     
    public:
        CTexture(int _iWidth, int _iHeight, const unsigned char* _uiData);

		const GLuint& GetTexture() const;
		void SetTexture(unsigned char* _uiData);
    };
}
    
#endif
