#include "io.hpp"
#include "render.hpp"
#include <iostream>

GLuint
load_shader(const char *path)
{
    GLuint type;
    if(strstr(path, ".vs")) {
        type = GL_VERTEX_SHADER;
    } else if(strstr(path, ".fs")) {
        type = GL_FRAGMENT_SHADER;
    } else if(strstr(path, ".gs")) {
        type = GL_GEOMETRY_SHADER;
    } else {
        std::cerr << "WARNING: Could not deduce shader type for "
                  << path
                  << ". Assuming fragment shader.";
        type = GL_FRAGMENT_SHADER;
    }
    
    std::string src = slurp_file(path);
    const char *c_src = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &c_src, nullptr);
    glCompileShader(shader);
    return shader;
}

GLuint
link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    return program;
}

GLuint
make_vbo(const void *data, size_t size, GLenum usage)
{
    GLuint vbo;
    GLint old_vbo;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &old_vbo);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, old_vbo);
    return vbo;
}


