#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include <GL/glext.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GLPointer.h"
#include "VertexMath.hpp"

/*! \class Shader
    \brief Class for creating shader program

    Contains vertex and fragment shaders
*/

class Shader
{
public:
    unsigned int iID;

    Shader(const char* vertexShaderPath_, const char* fragmentShaderPath_, const char* geometryShaderPath_ = nullptr)
    {
        std::string sVertex_Code;
        std::string sFragment_Code;
		std::string geometryShaderCode;
        std::ifstream Vertex_Shader_File;
        std::ifstream Fragment_Shader_File;
		std::ifstream geometryShaderFile;

        Vertex_Shader_File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        Fragment_Shader_File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            /// Open files
            Vertex_Shader_File.open(vertexShaderPath_);
            Fragment_Shader_File.open(fragmentShaderPath_);
            std::stringstream Vertex_Shader_Stream, Fragment_Shader_Stream;

            /// Read file buffers
            Vertex_Shader_Stream << Vertex_Shader_File.rdbuf();
            Fragment_Shader_Stream << Fragment_Shader_File.rdbuf();

            /// Close files
            Vertex_Shader_File.close();
            Fragment_Shader_File.close();

            /// Converting to string varibale thread data
            sVertex_Code = Vertex_Shader_Stream.str();
            sFragment_Code = Fragment_Shader_Stream.str();

			/// If geometry shader path is present, also load a geometry shader
			if (geometryShaderPath_ != nullptr) {
				geometryShaderFile.open(geometryShaderPath_);
				std::stringstream geometryShaderStream;
				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryShaderCode = geometryShaderStream.str();
			}
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* pVertex_Shader_Code   = sVertex_Code.c_str();
        const char* pFragment_Shader_Code = sFragment_Code.c_str();

        /// Shaders compilation
        GLuint uiVertex, uiFragment, uiGeometryShaderID;

        /// Vertex shader
        uiVertex = pGLCreate_Shader(GL_VERTEX_SHADER);
        pGLShader_Source(uiVertex, 1, &pVertex_Shader_Code, NULL);
        pGLCompile_Shader(uiVertex);
        CheckCompileErrors(uiVertex, "VERTEX");

        /// Fragment shader
        uiFragment = pGLCreate_Shader(GL_FRAGMENT_SHADER);
        pGLShader_Source(uiFragment, 1, &pFragment_Shader_Code, NULL);
        pGLCompile_Shader(uiFragment);
        CheckCompileErrors(uiFragment, "FRAGMENT");

		/// Geometry shader
		if (geometryShaderPath_ != nullptr) {
			const char* pGeometryShaderCode = geometryShaderCode.c_str();
			uiGeometryShaderID = pGLCreate_Shader(GL_GEOMETRY_SHADER);
			pGLShader_Source(uiGeometryShaderID, 1, &pGeometryShaderCode, NULL);
			pGLCompile_Shader(uiGeometryShaderID);
			CheckCompileErrors(uiGeometryShaderID, "GEOMETRY");
		}

        ///< Shader program
        iID = pGLCreate_Program();
        pGLAttach_Shader(iID, uiVertex);
        pGLAttach_Shader(iID, uiFragment);
		if (geometryShaderPath_ != nullptr)
			pGLAttach_Shader(iID, uiFragment);
        pGLLink_Program(iID);
        CheckCompileErrors(iID, "PROGRAM");

        ///< Free shaders
        pGLDelete_Shader(uiVertex);
        pGLDelete_Shader(uiFragment);
		if (geometryShaderPath_ != nullptr)
			pGLDelete_Shader(uiGeometryShaderID);
    }

    void Use();
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;
	void SetUniformID(const char* _uniformIdentificator, int _id);
	void SetMat4(const std::string &name, mat4 &mat) const;
	
private:
    void CheckCompileErrors(unsigned int shader, std::string type);
};

#endif
