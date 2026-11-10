#pragma once
#include "find_config.hpp"

#ifdef CBU_ENABLE_GL

#include <GL/gl.h>

GLuint glCreateShader(GLenum type);
void glShaderSource(GLuint shader, GLsizei count,
                    const GLchar *const *string, const GLint *length);
void glCompileShader(GLuint shader);
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length,
                        GLchar *infoLog);

GLuint glCreateProgram(void);
void glUseProgram(GLuint program);
void glAttachShader(GLuint program, GLuint shader);
void glLinkProgram(GLuint program);
void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length,
                         GLchar *infoLog);
void glDetachShader(GLuint program, GLuint shader);
void glDeleteShader(GLuint shader);

void glUniform1f(GLint location, GLfloat v0);
void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
void glUniform4f(GLint location, GLfloat v0, GLfloat v1,
                 GLfloat v2, GLfloat v3);

void glUniform1i(GLint location, GLint v0);

void glBindVertexArray(GLuint array);
void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glGenBuffers(GLsizei n, GLuint *buffers);
void glBindBuffer(GLenum target, GLuint buffer);
void glBufferData(GLenum target, GLsizeiptr size, const void *data,
                  GLenum usage);
void glEnableVertexAttribArray(GLuint index);
void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const void *pointer);

void glDeleteProgram(GLuint program);
GLint glGetUniformLocation(GLuint program, const GLchar *name);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glDeleteBuffers(GLsizei n, const GLuint *buffers);
void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);

typedef void (APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *message,
                                     const void *userParam);

void glDebugMessageCallback(GLDEBUGPROC callback, const void *userParam);
void glDebugMessageControl(GLenum source, GLenum type, GLenum severity,
                           GLsizei count, const GLuint *ids,
                           GLboolean enabled);

void glBindFramebuffer(GLenum target, GLuint framebuffer);
void glFramebufferTexture2D(GLenum target, GLenum attachment,
                            GLenum textarget, GLuint texture, GLint level);
void glDrawBuffers(GLsizei n, const GLenum *bufs);
void glGenFramebuffers(GLsizei n, GLuint *framebuffers);


void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);

void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB,
                         GLenum srcAlpha, GLenum dstAlpha);

void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count,
                           GLsizei instanceCount);

void glVertexAttribDivisor(GLuint index, GLuint divisor);


#endif
