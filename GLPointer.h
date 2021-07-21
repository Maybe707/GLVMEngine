#ifndef GLPOINTER
#define GLPOINTER

#include <GL/glx.h>

extern void (*pGLVertex_Arrays)(GLsizei, GLuint);

extern void (*pGLGen_Vertex_Arrays)(GLsizei, GLuint *);

extern void (*pGLGen_Buffers)(GLsizei, GLuint *);

extern void (*pGLBind_Vertex_Array)(GLuint);

extern void (*pGLBind_Buffer)(GLenum, GLuint);

extern void (*pGLBuffer_Data)(GLenum, GLsizeiptr, const GLvoid *, GLenum);

extern void (*pGLVertex_Attrib_Pointer)(GLuint, GLint, GLenum, GLboolean, GLsizei,
                                 const GLvoid *);

extern void (*pGLEnable_Vertex_Attrib_Array)(GLuint);

extern void (*pGen_Textures)(GLsizei, GLuint *);

extern void (*pGLBind_Textures)(GLuint, GLsizei, const GLuint *);

extern void (*pGLTex_Parameteri)(GLenum, GLenum,
                          GLint);

extern void (*pGLTex_Image2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum,
                       GLenum, const GLvoid *);

extern void (*pGLGenerate_Mipmap)(GLenum);

extern void (*pGLGet_Uniform_Location)(GLuint, const GLchar *);

extern void (*pGLUniform1i)(GLint, GLint);

extern void (*pGLDelete_Vertex_Arrays)(GLsizei, const GLuint *);

extern void (*pGLDelete_Buffers)(GLsizei,
                          const GLuint *);

extern GLuint (*pGLCreate_Shader)(GLenum);

extern void (*pGLShader_Source)(GLuint, GLsizei, const GLchar **, const GLint *);

extern void (*pGLCompile_Shader)(GLuint);

extern GLuint (*pGLCreate_Program)(void);

extern void (*pGLAttach_Shader)(GLuint, GLuint);

extern void (*pGLLink_Program)(GLuint);

extern void (*pGLDelete_Shader)(GLuint);

extern void (*pGLUse_Program)(GLuint);

extern void (*pGLUniform1f)(GLint, GLfloat);

extern void (*pGLGet_Shaderiv)(GLuint, GLenum,
                        GLint *);

extern void (*pGLGet_Shader_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *);

extern void (*pGLGet_Programiv)(GLuint, GLenum,
                         GLint *);

extern void (*pGLGet_Program_Info_Log)(GLuint, GLsizei, GLsizei, GLchar *);

#endif
