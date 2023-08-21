#include "io.hpp"
#include "render.hpp"
#include <iostream>

#define UNUSED(x) (void)(x)

static void
gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(id);
    UNUSED(severity);
    UNUSED(length);
    UNUSED(userParam);
    
    std::cerr << "GL: " << message << std::endl;
}

static void
error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error #" << error << ": "
              << description
              << std::endl;
}

GLFWwindow *
initWindow(void)
{
    GLFWwindow* window = nullptr;

    glfwSetErrorCallback(error_callback);

    if(!glfwInit()) {
        exit(1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if((window = glfwCreateWindow(640, 480, "Engine", nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        exit(1);
    }

    return window;
}

void
disposeWindow(GLFWwindow *&window)
{
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}

void
initRender(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, nullptr);

    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    std::cout << "OpenGL version: "
              << glGetString(GL_VERSION)
              << std::endl
              << "Shader Language: "
              << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;
}

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

GLuint
make_ebo(const void *data, size_t size, GLenum usage)
{
    GLuint ebo;
    GLint old_ebo;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_ebo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, old_ebo);
    return ebo;
}

GLuint
make_vao(void)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return vao;
}
