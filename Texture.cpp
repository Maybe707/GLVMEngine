#include "Texture.h"
#include <iostream>

CTexture::CTexture()
{
	glGenTextures(1, &iTexture_);
    glBindTexture(GL_TEXTURE_2D, iTexture_);
    // Set texture applying parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set applying texture method (GL_REPEAT - standart method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	uiData_ = stbi_load("../textures/enemy.png", &iWidth_, &iHeight_, &iNrChannels_, 0);
    if (uiData_)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth_, iHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, uiData_);
        pGLGenerate_Mipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(uiData_);


	pGLEnable_Vertex_Attrib_Array(1);
	pGLBind_Vertex_Array(0);
	pGLBind_Buffer(GL_ARRAY_BUFFER, 0);
}

CTexture::~CTexture()
{
}

GLuint* CTexture::GetTexture()
{
	return &iTexture_;
}
