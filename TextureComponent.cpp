#include "TextureComponent.hpp"
#include "ConstVectorContainer.hpp"
#include <GL/gl.h>

#define NUMBER_OF_CREATING_TEXTURE_OBJECT_1 1
#define SOME_STRANGE_STUFF 0
#define MIPMAP_LEVEL 0
#define SOME_OLD_STUFF 0

namespace GLVM::ECS
{   
    CTextureComponent::CTextureComponent()
    {
		///< Loading and creating texture.
		glGenTextures(NUMBER_OF_CREATING_TEXTURE_OBJECT_1, &iTexture_);
		glBindTexture(GL_TEXTURE_2D, iTexture_);
		///< Setting texture applying parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		///< Loading image, creating texture and generation mipmap-levels

		glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, iWidth_, iHeight_, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, u_iData_);
		pGLGenerate_Mipmap(GL_TEXTURE_2D);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
