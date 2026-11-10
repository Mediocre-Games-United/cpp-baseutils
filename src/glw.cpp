#include "glw.hpp"
#include "logger.hpp"
#include <SDL3/SDL.h>
#include <GL/gl.h>
#include <format>

#ifdef CBU_ENABLE_GL

template <typename func>
func glf_get_addr(const char *name) {
    auto addr = SDL_GL_GetProcAddress(name);
    if (!addr) cbu::log_error(true,std::format("GLW error: {} does not exist",name));

    return (func) addr;
}
#define GET_ADDR(name,ret,vals) glf_get_addr<ret (APIENTRYP)vals>(name)

GLuint glCreateShader(GLenum type) {
    static auto addr = GET_ADDR("glCreateShader",GLuint,(GLenum type));
    return addr(type);
}


void glShaderSource(GLuint shader, GLsizei count,const GLchar *const *string, const GLint *length) {
    static auto addr = GET_ADDR("glShaderSource",void,(GLuint shader, GLsizei count,const GLchar *const *string, const GLint *length));
    return addr(shader,count,string,length);
}

void glCompileShader(GLuint shader) {
    static auto addr = GET_ADDR("glCompileShader", void, (GLuint shader));
    addr(shader);
}

void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
    static auto addr = GET_ADDR("glGetShaderiv", void,
                                (GLuint shader, GLenum pname, GLint *params));
    addr(shader, pname, params);
}

void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length,
                        GLchar *infoLog) {
    static auto addr = GET_ADDR("glGetShaderInfoLog", void,
                                (GLuint shader, GLsizei maxLength,
                                 GLsizei *length, GLchar *infoLog));
    addr(shader, maxLength, length, infoLog);
}

GLuint glCreateProgram(void) {
    static auto addr = GET_ADDR("glCreateProgram", GLuint, (void));
    return addr();
}

void glUseProgram(GLuint program) {
    static auto addr = GET_ADDR("glUseProgram", void, (GLuint program));
    addr(program);
}

void glAttachShader(GLuint program, GLuint shader) {
    static auto addr = GET_ADDR("glAttachShader", void,
                                (GLuint program, GLuint shader));
    addr(program, shader);
}

void glLinkProgram(GLuint program) {
    static auto addr = GET_ADDR("glLinkProgram", void, (GLuint program));
    addr(program);
}

void glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
    static auto addr = GET_ADDR("glGetProgramiv", void,
                                (GLuint program, GLenum pname, GLint *params));
    addr(program, pname, params);
}

void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length,
                         GLchar *infoLog) {
    static auto addr = GET_ADDR("glGetProgramInfoLog", void,
                                (GLuint program, GLsizei maxLength,
                                 GLsizei *length, GLchar *infoLog));
    addr(program, maxLength, length, infoLog);
}

void glDetachShader(GLuint program, GLuint shader) {
    static auto addr = GET_ADDR("glDetachShader", void,
                                (GLuint program, GLuint shader));
    addr(program, shader);
}

void glDeleteShader(GLuint shader) {
    static auto addr = GET_ADDR("glDeleteShader", void, (GLuint shader));
    addr(shader);
}

void glUniform1f(GLint location, GLfloat v0) {
    static auto addr = GET_ADDR("glUniform1f", void,
                                (GLint location, GLfloat v0));
    addr(location, v0);
}

void glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
    static auto addr = GET_ADDR("glUniform2f", void,
                                (GLint location, GLfloat v0, GLfloat v1));
    addr(location, v0, v1);
}

void glUniform4f(GLint location, GLfloat v0, GLfloat v1,
                 GLfloat v2, GLfloat v3) {
    static auto addr = GET_ADDR("glUniform4f", void,
                                (GLint location, GLfloat v0, GLfloat v1,
                                 GLfloat v2, GLfloat v3));
    addr(location, v0, v1, v2, v3);
}

void glUniform1i(GLint location, GLint v0) {
    static auto addr = GET_ADDR("glUniform1i", void,
                                (GLint location, GLint v0));
    addr(location, v0);
}

void glBindVertexArray(GLuint array) {
    static auto addr = GET_ADDR("glBindVertexArray", void, (GLuint array));
    addr(array);
}

void glGenVertexArrays(GLsizei n, GLuint *arrays) {
    static auto addr = GET_ADDR("glGenVertexArrays", void,
                                (GLsizei n, GLuint *arrays));
    addr(n, arrays);
}

void glGenBuffers(GLsizei n, GLuint *buffers) {
    static auto addr = GET_ADDR("glGenBuffers", void,
                                (GLsizei n, GLuint *buffers));
    addr(n, buffers);
}

void glBindBuffer(GLenum target, GLuint buffer) {
    static auto addr = GET_ADDR("glBindBuffer", void,
                                (GLenum target, GLuint buffer));
    addr(target, buffer);
}

void glBufferData(GLenum target, GLsizeiptr size, const void *data,
                  GLenum usage) {
    static auto addr = GET_ADDR("glBufferData", void,
                                (GLenum target, GLsizeiptr size,
                                 const void *data, GLenum usage));
    addr(target, size, data, usage);
}

void glEnableVertexAttribArray(GLuint index) {
    static auto addr = GET_ADDR("glEnableVertexAttribArray", void,
                                (GLuint index));
    addr(index);
}

void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const void *pointer) {
    static auto addr = GET_ADDR("glVertexAttribPointer", void,
                                (GLuint index, GLint size, GLenum type,
                                 GLboolean normalized, GLsizei stride,
                                 const void *pointer));
    addr(index, size, type, normalized, stride, pointer);
}

void glDeleteProgram(GLuint program) {
    static auto addr = GET_ADDR("glDeleteProgram", void, (GLuint program));
    addr(program);
}

GLint glGetUniformLocation(GLuint program, const GLchar *name) {
    static auto addr = GET_ADDR("glGetUniformLocation", GLint,
                                (GLuint program, const GLchar *name));
    return addr(program, name);
}

void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
    static auto addr = GET_ADDR("glDeleteVertexArrays", void,
                                (GLsizei n, const GLuint *arrays));
    addr(n, arrays);
}

void glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    static auto addr = GET_ADDR("glDeleteBuffers", void,
                                (GLsizei n, const GLuint *buffers));
    addr(n, buffers);
}

void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
    static auto addr = GET_ADDR("glDeleteFramebuffers", void,
                                (GLsizei n, const GLuint *framebuffers));
    addr(n, framebuffers);
}

void glDebugMessageCallback(GLDEBUGPROC callback, const void *userParam) {
    static auto addr = GET_ADDR("glDebugMessageCallback", void,
                                (GLDEBUGPROC callback,
                                 const void *userParam));
    addr(callback, userParam);
}

void glDebugMessageControl(GLenum source, GLenum type, GLenum severity,
                           GLsizei count, const GLuint *ids,
                           GLboolean enabled) {
    static auto addr = GET_ADDR("glDebugMessageControl", void,
                                (GLenum source, GLenum type, GLenum severity,
                                 GLsizei count, const GLuint *ids,
                                 GLboolean enabled));
    addr(source, type, severity, count, ids, enabled);
}

void glBindFramebuffer(GLenum target, GLuint framebuffer) {
    static auto addr = GET_ADDR("glBindFramebuffer", void,
                                (GLenum target, GLuint framebuffer));
    addr(target, framebuffer);
}

void glFramebufferTexture2D(GLenum target, GLenum attachment,
                            GLenum textarget, GLuint texture, GLint level) {
    static auto addr = GET_ADDR("glFramebufferTexture2D", void,
                                (GLenum target, GLenum attachment,
                                 GLenum textarget, GLuint texture,
                                 GLint level));
    addr(target, attachment, textarget, texture, level);
}

void glDrawBuffers(GLsizei n, const GLenum *bufs) {
    static auto addr = GET_ADDR("glDrawBuffers", void,
                                (GLsizei n, const GLenum *bufs));
    addr(n, bufs);
}

void glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    static auto addr = GET_ADDR("glGenFramebuffers", void,
                                (GLsizei n, GLuint *framebuffers));
    addr(n, framebuffers);
}

void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
    static auto addr = GET_ADDR("glBlendEquationSeparate", void,
                                (GLenum modeRGB, GLenum modeAlpha));
    addr(modeRGB, modeAlpha);
}

void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB,
                         GLenum srcAlpha, GLenum dstAlpha) {
    static auto addr = GET_ADDR("glBlendFuncSeparate", void,
                                (GLenum srcRGB, GLenum dstRGB,
                                 GLenum srcAlpha, GLenum dstAlpha));
    addr(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count,
                           GLsizei instanceCount) {
    static auto addr = GET_ADDR("glDrawArraysInstanced", void,
                                (GLenum mode, GLint first, GLsizei count,
                                 GLsizei instanceCount));
    addr(mode, first, count, instanceCount);
}

void glVertexAttribDivisor(GLuint index, GLuint divisor) {
    static auto addr = GET_ADDR("glVertexAttribDivisor", void,
                                (GLuint index, GLuint divisor));
    addr(index, divisor);
}



#endif
