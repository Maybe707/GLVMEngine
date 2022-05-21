#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GLPointer.h"

/*! \class Shader
    \brief Class for creating shader program

    Contains vertex and fragment shaders
*/

class Shader
{
public:
    unsigned int iID;

    Shader(const char* _kVertex_Shader_Path, const char* _kFragment_Shader_Path)
    {
		const char* _pVertex_Path = _kVertex_Shader_Path;
		const char* _pFragment_Path = _kFragment_Shader_Path;
        std::string sVertex_Code;
        std::string sFragment_Code;
        std::ifstream Vertex_Shader_File;
        std::ifstream Fragment_Shader_File;

        Vertex_Shader_File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        Fragment_Shader_File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            ///< Open files
            Vertex_Shader_File.open(_pVertex_Path);
            Fragment_Shader_File.open(_pFragment_Path);
            std::stringstream Vertex_Shader_Stream, Fragment_Shader_Stream;

            ///< Read file buffers
            Vertex_Shader_Stream << Vertex_Shader_File.rdbuf();
            Fragment_Shader_Stream << Fragment_Shader_File.rdbuf();

            ///< Close files
            Vertex_Shader_File.close();
            Fragment_Shader_File.close();

            ///< Converting to string varibale thread data
            sVertex_Code = Vertex_Shader_Stream.str();
            sFragment_Code = Fragment_Shader_Stream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* pVertex_Shader_Code = sVertex_Code.c_str();
        const char* pFragment_Shader_Code = sFragment_Code.c_str();

        ///< Shaders compilation
        GLuint uiVertex, uiFragment;

        ///< Vertex shader
        uiVertex = pGLCreate_Shader(GL_VERTEX_SHADER);
        pGLShader_Source(uiVertex, 1, &pVertex_Shader_Code, NULL);
        pGLCompile_Shader(uiVertex);
        CheckCompileErrors(uiVertex, "VERTEX");

        ///< Fragment shader
        uiFragment = pGLCreate_Shader(GL_FRAGMENT_SHADER);
        pGLShader_Source(uiFragment, 1, &pFragment_Shader_Code, NULL);
        pGLCompile_Shader(uiFragment);
        CheckCompileErrors(uiFragment, "FRAGMENT");

        ///< Shader program
        iID = pGLCreate_Program();
        pGLAttach_Shader(iID, uiVertex);
        pGLAttach_Shader(iID, uiFragment);
        pGLLink_Program(iID);
        CheckCompileErrors(iID, "PROGRAM");

        ///< Free shaders
        pGLDelete_Shader(uiVertex);
        pGLDelete_Shader(uiFragment);
    }

    void Use();
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
	void SetUniformID();
	
private:
    void CheckCompileErrors(unsigned int shader, std::string type);
};

#endif
