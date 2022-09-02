#ifndef TEXTURE
#define TEXTURE

#include <vector>
#include <GL/gl.h>

typedef unsigned int Entity;

namespace GLVM::ECS
{
    struct CTexture
    {
        GLuint iTexture_;
        std::vector<Entity> entitiesOwnsThisTypeOfTexture_;
        unsigned int id_;
		unsigned int iWidth_;
		unsigned int iHeight_;
        unsigned int dat_length_;
		const unsigned char* u_iData_;
    };
}

#endif
