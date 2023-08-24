#ifndef RENDER_HPP_INCLUDED
#define RENDER_HPP_INCLUDED

#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

GLFWwindow *initWindow(void);
void        initRender(GLFWwindow *);
void        disposeWindow(GLFWwindow *&);

struct Texture
{
private:
    GLuint    _texture;
    glm::vec2 _size;
public:
    static Texture load(const char *path);
    void           bind(void);
    glm::vec2      getSize(void);
    void           dispose(void);
};

struct ShaderProgram
{
private:
    GLuint _program;
public:
    static ShaderProgram link(const std::vector<const char*> shaderpaths);
    void                 use(void);
    GLuint               getAttribLocation(const char *name);
    GLuint               getUniformLocation(const char *name);
    void                 dispose(void);
};

GLuint  load_texture(const char *path, glm::vec2& size);
GLuint  load_shader(const char *path);
GLuint  link_program(GLuint vs, GLuint fs);
GLuint  make_vbo(const void *data, size_t size, GLenum usage);
GLuint  make_vao(void);
GLuint  make_ebo(const void *data, size_t size, GLenum usage);

#endif
