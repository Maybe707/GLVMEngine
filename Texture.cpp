#include "Texture.hpp"
#include <GL/gl.h>

#define NUMBER_OF_CREATING_TEXTURE_OBJECT_1 1
#define SOME_STRANGE_STUFF 0
#define MIPMAP_LEVEL 0
#define SOME_OLD_STUFF 0

namespace GLVM::Core
{   
    CTexture::CTexture(int _iWidth, int _iHeight, const unsigned char* _uiData)
    {
		iWidth_ = _iWidth;
		iHeight_ = _iHeight;
		uiData_ = _uiData;
		
		///< Loading and creating texture.
		glGenTextures(NUMBER_OF_CREATING_TEXTURE_OBJECT_1, &iTexture_);
		glBindTexture(GL_TEXTURE_2D, iTexture_);
		///< Setting texture applying parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		///< Loading image, creating texture and generation mipmap-levels

		glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, iWidth_, iHeight_, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, _uiData);
		pGLGenerate_Mipmap(GL_TEXTURE_2D);
		
		// uiData_ = stbi_load(_cImage_Path, &iWidth_, &iHeight_, &iNrChannels_, SOME_STRANGE_STUFF);
		// if (uiData_)
		// {
		// 	glTexImage2D(GL_TEXTURE_2D, MIPMAP_LEVEL, GL_RGBA, 96, 128, SOME_OLD_STUFF, GL_RGBA, GL_UNSIGNED_BYTE, chelik_dat);
		// 	pGLGenerate_Mipmap(GL_TEXTURE_2D);
		// }
		// else
		// {
		// 	std::cout << "Failed to load texture" << std::endl;
		// }
		// stbi_image_free(uiData_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

	const GLuint& CTexture::GetTexture() const
	{
		return iTexture_;
	}

	void CTexture::SetTexture(unsigned char* _uiData)
	{
		uiData_ = _uiData;
	}
}
