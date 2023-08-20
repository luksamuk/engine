#ifndef RENDER_HPP_INCLUDED
#define RENDER_HPP_INCLUDED

#include <cstring>
#include <glad/glad.h>

GLuint  load_shader(const char *path);
GLuint  link_program(GLuint vs, GLuint fs);
GLuint  make_vbo(const void *data, size_t size, GLenum usage);

#endif
