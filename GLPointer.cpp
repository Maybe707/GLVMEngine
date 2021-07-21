#include <GL/glx.h>

#ifdef __linux__
#define GET_PROC_ADDRESS glXGetProcAddress
#endif

#ifdef _WIN32
#define GET_PROC_ADDRESS wglGetProcAddress
#endif

void (*pGLVertex_Arrays)(GLsizei, GLuint) = (void (*)(GLsizei, GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glGenVertexArrays");

void (*pGLGen_Vertex_Arrays)(GLsizei, GLuint *) =(void (*)(GLsizei, GLuint *))
    GET_PROC_ADDRESS((const GLubyte *)"glGenVertexArrays");

void (*pGLGen_Buffers)(GLsizei, GLuint *) = (void (*)(GLsizei, GLuint *))
    GET_PROC_ADDRESS((const GLubyte *)"glGenBuffers");

void (*pGLBind_Vertex_Array)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glBindVertexArray");

void (*pGLBind_Buffer)(GLenum, GLuint) = (void (*)(GLenum, GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glBindBuffer");

void (*pGLBuffer_Data)(GLenum, GLsizeiptr, const GLvoid *, GLenum) =
    (void (*)(GLenum, GLsizeiptr, const GLvoid *,
              GLenum))GET_PROC_ADDRESS((const GLubyte *)"glBufferData");

void (*pGLVertex_Attrib_Pointer)(GLuint, GLint, GLenum, GLboolean, GLsizei,
                                 const GLvoid *) =
    (void (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))
        GET_PROC_ADDRESS((const GLubyte *)"glVertexAttribPointer");

void (*pGLEnable_Vertex_Attrib_Array)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glEnableVertexAttribArray");

void (*pGen_Textures)(GLsizei, GLuint *) = (void (*)(GLsizei, GLuint *))
    GET_PROC_ADDRESS((const GLubyte *)"glGenTextures");

void (*pGLBind_Textures)(GLuint, GLsizei, const GLuint *) = (void (*)(
    GLuint, GLsizei,
    const GLuint *))GET_PROC_ADDRESS((const GLubyte *)"glBindTextures");

void (*pGLTex_Parameteri)(GLenum, GLenum,
                          GLint) = (void (*)(GLenum, GLenum, GLint))
    GET_PROC_ADDRESS((const GLubyte *)"glTexParameteri");

void (*pGLTex_Image2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum,
                       GLenum, const GLvoid *) =
    (void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum,
              const GLvoid *))
        GET_PROC_ADDRESS((const GLubyte *)"glTexImage2D");

void (*pGLGenerate_Mipmap)(GLenum) = (void (*)(GLenum))
    GET_PROC_ADDRESS((const GLubyte *)"glGenerateMipmap");

void (*pGLGet_Uniform_Location)(GLuint, const GLchar *) = (void (*)(
    GLuint,
    const GLchar *))GET_PROC_ADDRESS((const GLubyte *)"glGetUniformLocation");

void (*pGLUniform1i)(GLint, GLint) = (void (*)(GLint, GLint))
    GET_PROC_ADDRESS((const GLubyte *)"glUniform1i");

void (*pGLDelete_Vertex_Arrays)(GLsizei, const GLuint *) = (void (*)(
    GLsizei,
    const GLuint *))GET_PROC_ADDRESS((const GLubyte *)"glDeleteVertexArrays");

void (*pGLDelete_Buffers)(GLsizei,
                          const GLuint *) = (void (*)(GLsizei, const GLuint *))
    GET_PROC_ADDRESS((const GLubyte *)"glDeleteBuffers");

GLuint (*pGLCreate_Shader)(GLenum) = (GLuint(*)(GLenum))
    GET_PROC_ADDRESS((GLubyte *)"glCreateShader");

void (*pGLShader_Source)(GLuint, GLsizei, const GLchar **, const GLint *) =
    (void (*)(GLuint, GLsizei, const GLchar **, const GLint *))
        GET_PROC_ADDRESS((const GLubyte *)"glShaderSource");

void (*pGLCompile_Shader)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glCompileShader");

GLuint (*pGLCreate_Program)(void) = (GLuint(*)(void))
    GET_PROC_ADDRESS((const GLubyte *)"glCreateProgram");

void (*pGLAttach_Shader)(GLuint, GLuint) = (void (*)(GLuint, GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glAttachShader");

void (*pGLLink_Program)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glLinkProgram");

void (*pGLDelete_Shader)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glDeleteShader");

void (*pGLUse_Program)(GLuint) = (void (*)(GLuint))
    GET_PROC_ADDRESS((const GLubyte *)"glUseProgram");

void (*pGLUniform1f)(GLint, GLfloat) = (void (*)(GLint, GLfloat))
    GET_PROC_ADDRESS((const GLubyte *)"glUniform1f");

void (*pGLGet_Shaderiv)(GLuint, GLenum,
                       GLint *) = (void (*)(GLuint, GLenum, GLint *))
    GET_PROC_ADDRESS((const GLubyte *)"glGetShaderiv");

void (*pGLGet_Shader_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *) = (void (*)(
    GLuint, GLsizei, GLsizei,
    GLchar *))GET_PROC_ADDRESS((const GLubyte *)"glGetShaderInfoLog");

void (*pGLGet_Programiv)(GLuint, GLenum,
                         GLint *) = (void (*)(GLuint, GLenum, GLint *))
    GET_PROC_ADDRESS((const GLubyte *)"glGetProgramiv");

void (*pGLGet_Program_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *) = (void (*)(
    GLuint, GLsizei, GLsizei,
    GLchar *))GET_PROC_ADDRESS((const GLubyte *)"glGetProgramInfoLog");
