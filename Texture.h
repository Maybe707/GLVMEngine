#include <GL/gl.h>
#include "GLPointer.h"
#include "stb_image.h"

class CTexture
{
    GLuint iTexture_;
	int iWidth_;
	int iHeight_;
	int iNrChannels_;
	unsigned char* uiData_;

public:
    CTexture();
    ~CTexture();

	GLuint* GetTexture();
};
    
	
     
