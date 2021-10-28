#ifndef TEXTURE
#define TEXTURE

#include <GL/gl.h>
#include "GLPointer.h"
#include "ShaderProgram.hpp"
#include <iostream>
#include "IComponent.hpp"
#include "ConstVectorContainer.hpp"

namespace GLVM::Core
{

    /*! \class Sprite
        \brief Creates sprites for game objects.
    */
    
    class CTexture : public ECS::IComponent
    {
		static TCConstVectorContainer<CTexture*> s_tTexture_Components_Array;
		GLuint iTexture_;
		unsigned int iWidth_;
		unsigned int iHeight_;
		unsigned int iNrChannels_;
		const unsigned char* uiData_;
     
    public:
        CTexture(unsigned int _iWidth, unsigned int _iHeight, const unsigned char* _uiData, const unsigned int _Entity_ID);

		const GLuint& GetTexture() const;
		void SetTexture(unsigned char* _uiData);
		TCConstVectorContainer<CTexture*>& GetTextureComponentsArray();
    };
}
    
#endif
