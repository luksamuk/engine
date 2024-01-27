#ifndef RENDER_HPP_INCLUDED
#define RENDER_HPP_INCLUDED

#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Render
{
    void        init(GLFWwindow *);
    void        initGui(GLFWwindow *);
    GLFWwindow *initWindow(std::string caption);
    void        disposeWindow(GLFWwindow *&);
    
    struct Texture
    {
    private:
        GLuint    _texture;
        glm::vec2 _size;
    public:
        ~Texture();
        static Texture *load(const char *path);
        void            bind(void);
        glm::vec2       getSize(void);
        void            dispose(void);
    };

    struct ShaderProgram
    {
    private:
        GLuint _program;
    public:
        ~ShaderProgram();
        static ShaderProgram *link(const std::vector<const char*> shaderpaths);
        void                  use(void);
        GLuint                getAttribLocation(const char *name);
        GLuint                getUniformLocation(const char *name);
        void                  dispose(void);
    };

    struct QuadGeometry
    {
    private:
        virtual ~QuadGeometry() = 0;
    public:
        static const float *        vertices(void);
        static const unsigned int * elements(void);
        static unsigned int         numVertices(void);
        static unsigned int         numElements(void);
        static unsigned int         numVertexComponents(void);
        static unsigned int         verticesSize(void);
        static unsigned int         singleVertexSize(void);
        static unsigned int         elementsSize(void);
    };

    GLuint      raw_load_texture(const char *path, glm::vec2& size);
    GLuint      raw_load_shader(const char *path);
    GLuint      raw_link_program(GLuint vs, GLuint fs);
    GLuint      make_vbo(const void *data, size_t size, GLenum usage);
    GLuint      make_vao(void);
    GLuint      make_ebo(const void *data, size_t size, GLenum usage);
    glm::ivec2  windowSize();

    void    setClearColor(glm::vec4 color);
}

#endif
