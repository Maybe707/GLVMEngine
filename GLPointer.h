#include <GL/glx.h>

void (*pGLVertex_Arrays)(GLsizei, GLuint) = (void (*)(GLsizei, GLuint))
    glXGetProcAddress((const GLubyte *)"glGenVertexArrays");

void (*pGLGen_Vertex_Arrays)(GLsizei, GLuint *) =(void (*)(GLsizei, GLuint *))
    glXGetProcAddress((const GLubyte *)"glGenVertexArrays");

void (*pGLGen_Buffers)(GLsizei, GLuint *) = (void (*)(GLsizei, GLuint *))
    glXGetProcAddress((const GLubyte *)"glGenBuffers");

void (*pGLBind_Vertex_Array)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glBindVertexArray");

void (*pGLBind_Buffer)(GLenum, GLuint) = (void (*)(GLenum, GLuint))
    glXGetProcAddress((const GLubyte *)"glBindBuffer");

void (*pGLBuffer_Data)(GLenum, GLsizeiptr, const GLvoid *, GLenum) =
    (void (*)(GLenum, GLsizeiptr, const GLvoid *,
              GLenum))glXGetProcAddress((const GLubyte *)"glBufferData");

void (*pGLVertex_Attrib_Pointer)(GLuint, GLint, GLenum, GLboolean, GLsizei,
                                 const GLvoid *) =
    (void (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))
        glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");

void (*pGLEnable_Vertex_Attrib_Array)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");

void (*pGen_Textures)(GLsizei, GLuint *) = (void (*)(GLsizei, GLuint *))
    glXGetProcAddress((const GLubyte *)"glGenTextures");

void (*pGLBind_Textures)(GLuint, GLsizei, const GLuint *) = (void (*)(
    GLuint, GLsizei,
    const GLuint *))glXGetProcAddress((const GLubyte *)"glBindTextures");

void (*pGLTex_Parameteri)(GLenum, GLenum,
                          GLint) = (void (*)(GLenum, GLenum, GLint))
    glXGetProcAddress((const GLubyte *)"glTexParameteri");

void (*pGLTex_Image2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum,
                       GLenum, const GLvoid *) =
    (void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum,
              const GLvoid *))
        glXGetProcAddress((const GLubyte *)"glTexImage2D");

void (*pGLGenerate_Mipmap)(GLenum) = (void (*)(GLenum))
    glXGetProcAddress((const GLubyte *)"glGenerateMipmap");

void (*pGLGet_Uniform_Location)(GLuint, const GLchar *) = (void (*)(
    GLuint,
    const GLchar *))glXGetProcAddress((const GLubyte *)"glGetUniformLocation");

void (*pGLUniform1i)(GLint, GLint) = (void (*)(GLint, GLint))
    glXGetProcAddress((const GLubyte *)"glUniform1i");

void (*pGLDelete_Vertex_Arrays)(GLsizei, const GLuint *) = (void (*)(
    GLsizei,
    const GLuint *))glXGetProcAddress((const GLubyte *)"glDeleteVertexArrays");

void (*pGLDelete_Buffers)(GLsizei,
                          const GLuint *) = (void (*)(GLsizei, const GLuint *))
    glXGetProcAddress((const GLubyte *)"glDeleteBuffers");

GLuint (*pGLCreate_Shader)(GLenum) = (GLuint(*)(GLenum))
    glXGetProcAddress((GLubyte *)"glCreateShader");

void (*pGLShader_Source)(GLuint, GLsizei, const GLchar **, const GLint *) =
    (void (*)(GLuint, GLsizei, const GLchar **, const GLint *))
        glXGetProcAddress((const GLubyte *)"glShaderSource");

void (*pGLCompile_Shader)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glCompileShader");

GLuint (*pGLCreate_Program)(void) = (GLuint(*)(void))
    glXGetProcAddress((const GLubyte *)"glCreateProgram");

void (*pGLAttach_Shader)(GLuint, GLuint) = (void (*)(GLuint, GLuint))
    glXGetProcAddress((const GLubyte *)"glAttachShader");

void (*pGLLink_Program)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glLinkProgram");

void (*pGLDelete_Shader)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glDeleteShader");

void (*pGLUse_Program)(GLuint) = (void (*)(GLuint))
    glXGetProcAddress((const GLubyte *)"glUseProgram");

void (*pGLUniform1f)(GLint, GLfloat) = (void (*)(GLint, GLfloat))
    glXGetProcAddress((const GLubyte *)"glUniform1f");

void (*pGLGet_Shaderiv)(GLuint, GLenum,
                       GLint *) = (void (*)(GLuint, GLenum, GLint *))
    glXGetProcAddress((const GLubyte *)"glGetShaderiv");

void (*pGLGet_Shader_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *) = (void (*)(
    GLuint, GLsizei, GLsizei,
    GLchar *))glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");

void (*pGLGet_Programiv)(GLuint, GLenum,
                         GLint *) = (void (*)(GLuint, GLenum, GLint *))
    glXGetProcAddress((const GLubyte *)"glGetProgramiv");

void (*pGLGet_Program_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *) = (void (*)(
    GLuint, GLsizei, GLsizei,
    GLchar *))glXGetProcAddress((const GLubyte *)"glGetProgramInfoLog");
