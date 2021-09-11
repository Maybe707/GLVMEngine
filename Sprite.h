#ifndef SPRITE
#define SPRITE

#include <GL/gl.h>
#include "GLPointer.h"
#include "GLPointer.h"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <GL/gl.h>
#include <iostream>

//#define VERTEX_ARRAY_RANGE 30

namespace GLVM::Core
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CSprite
    {
//        GLuint iVbo_;
//        GLuint iVao_;
		GLuint iTexture_;
		int iWidth_;
		int iHeight_;
		int iNrChannels_;
		unsigned char* uiData_;
//        float aVertices_[VERTEX_ARRAY_RANGE];
     
    public:
        CSprite(const char* _cImage_Path);
  
		void BindTexture();
	    void ActiveTexture();
    };
}
    
#endif
