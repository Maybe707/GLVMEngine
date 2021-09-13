#include "ShaderProgram.hpp"

#define ARRAY_INFO_LOG_RANGE 1024

///< Activate shader program
void Shader::Use()
{
	pGLUse_Program(iID);
}
///< Uniform functions
void Shader::SetBool(const std::string& name, bool value) const
{
	pGLUniform1i(pGLGet_Uniform_Location(iID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string& name, int value) const
{
	pGLUniform1i(pGLGet_Uniform_Location(iID, name.c_str()), value);
}
void Shader::SetFloat(const std::string& name, float value) const
{
	pGLUniform1f(pGLGet_Uniform_Location(iID, name.c_str()), value);
}
void Shader::SetUniformID()
{
	pGLUniform1i(pGLGet_Uniform_Location(iID, "tex"), 10);
}
	
///< Functions for exmination of shaders errors
void Shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int iSuccess;
	char cInfo_Log[ARRAY_INFO_LOG_RANGE];
	if (type != "PROGRAM")
	{
		pGLGet_Shaderiv(shader, GL_COMPILE_STATUS, &iSuccess);
		if (!iSuccess)
		{
			pGLGet_Shader_Info_Log(shader, ARRAY_INFO_LOG_RANGE, 0, cInfo_Log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << cInfo_Log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		pGLGet_Programiv(shader, GL_LINK_STATUS, &iSuccess);
		if (!iSuccess)
		{
			pGLGet_Program_Info_Log(shader, ARRAY_INFO_LOG_RANGE, 0, cInfo_Log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << cInfo_Log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
