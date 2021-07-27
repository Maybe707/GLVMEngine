#ifndef GLPOINTER
#define GLPOINTER

#ifdef __linux__
#include <GL/glx.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifdef INIT_EXT
#define EXTERN
#else
#define EXTERN extern
#endif

void Initializer();

EXTERN void (*pGLVertex_Arrays)(GLsizei, GLuint);

EXTERN void (*pGLGen_Vertex_Arrays)(GLsizei, GLuint *);

EXTERN void (*pGLGen_Buffers)(GLsizei, GLuint *);

EXTERN void (*pGLBind_Vertex_Array)(GLuint);

EXTERN void (*pGLBind_Buffer)(GLenum, GLuint);

EXTERN void (*pGLBuffer_Data)(GLenum, GLsizeiptr, const GLvoid *, GLenum);

EXTERN void (*pGLVertex_Attrib_Pointer)(GLuint, GLint, GLenum, GLboolean, GLsizei,
                                 const GLvoid *);

EXTERN void (*pGLEnable_Vertex_Attrib_Array)(GLuint);

EXTERN void (*pGen_Textures)(GLsizei, GLuint *);

EXTERN void (*pGLBind_Textures)(GLuint, GLsizei, const GLuint *);

EXTERN void (*pGLTex_Parameteri)(GLenum, GLenum,
                          GLint);

EXTERN void (*pGLTex_Image2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum,
                       GLenum, const GLvoid *);

EXTERN void (*pGLGenerate_Mipmap)(GLenum);

EXTERN GLint (*pGLGet_Uniform_Location)(GLuint, const GLchar *);

EXTERN void (*pGLUniform1i)(GLint, GLint);

EXTERN void (*pGLDelete_Vertex_Arrays)(GLsizei, const GLuint *);

EXTERN void (*pGLDelete_Buffers)(GLsizei,
                          const GLuint *);

EXTERN GLuint (*pGLCreate_Shader)(GLenum);

EXTERN void (*pGLShader_Source)(GLuint, GLsizei, const GLchar **, const GLint *);

EXTERN void (*pGLCompile_Shader)(GLuint);

EXTERN GLuint (*pGLCreate_Program)(void);

EXTERN void (*pGLAttach_Shader)(GLuint, GLuint);

EXTERN void (*pGLLink_Program)(GLuint);

EXTERN void (*pGLDelete_Shader)(GLuint);

EXTERN void (*pGLUse_Program)(GLuint);

EXTERN void (*pGLUniform1f)(GLint, GLfloat);

EXTERN void (*pGLGet_Shaderiv)(GLuint, GLenum,
                        GLint *);

EXTERN void (*pGLGet_Shader_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *);

EXTERN void (*pGLGet_Programiv)(GLuint, GLenum,
                         GLint *);

EXTERN void (*pGLGet_Program_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *);

#endif
