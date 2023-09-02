#include "io.hpp"
#include "render.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define UNUSED(x) (void)(x)

namespace Render
{
    //const glm::ivec2 winSize(320, 224);
    const glm::ivec2 winSize = glm::ivec2(320, 224) * 3;
    const float aspectRatio = (float)winSize.x / (float)winSize.y;

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

    static void
    window_size_callback(GLFWwindow *window, int width, int height)
    {
        UNUSED(window);
        //glViewport(0,0, width, height);
        int targetWidth = (int)glm::trunc(height * aspectRatio);
        auto newX = (width - targetWidth) / 2;
        glViewport(newX, 0, targetWidth, height);
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

        if((window = glfwCreateWindow(winSize.x, winSize.y, "Engine", nullptr, nullptr)) == nullptr)
        {
            glfwTerminate();
            exit(1);
        }

        glfwSetWindowSizeCallback(window, window_size_callback);

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
    init(GLFWwindow *window)
    {
        glfwMakeContextCurrent(window);
        gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_callback, nullptr);

        glEnable(GL_DEPTH_TEST);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    
        glViewport(0, 0, winSize.x, winSize.y);

        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
    
        std::cout << "OpenGL version: "
                  << glGetString(GL_VERSION)
                  << std::endl
                  << "Shader Language: "
                  << glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << std::endl;
    };

    GLuint
    load_texture(const char *path, glm::vec2& size)
    {
        GLuint tex;
        GLint old_tex;
        glGenTextures(1, &tex);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &old_tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        int width, height, channels;
        unsigned char *data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
        GLuint comp = (channels == 3) ? GL_RGB : (channels == 4) ? GL_RGBA : 0;
        glTexImage2D(GL_TEXTURE_2D, 0, comp, width, height, 0, comp, GL_UNSIGNED_BYTE, data);
        glFinish();
        stbi_image_free(data);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, old_tex);

        size = glm::vec2((float)width, (float)height);
        return tex;
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
    
        std::string src = IO::slurp_file(path);
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


    Texture
    Texture::load(const char *path)
    {
        Texture t;
        t._texture = load_texture(path, t._size);
        return t;
    }

    void
    Texture::bind(void)
    {
        glBindTexture(GL_TEXTURE_2D, this->_texture);
    }

    glm::vec2
    Texture::getSize(void)
    {
        return this->_size;
    }

    void
    Texture::dispose()
    {
        glDeleteTextures(1, &this->_texture);
    }

    ShaderProgram
    ShaderProgram::link(const std::vector<const char*> shaderpaths)
    {
        ShaderProgram p;
        std::vector<GLuint> shaders;
        p._program = glCreateProgram();
        for(auto path : shaderpaths) {
            GLuint shader = load_shader(path);
            glAttachShader(p._program, shader);
            shaders.push_back(shader);
        }
        glLinkProgram(p._program);
        for(auto shader : shaders) {
            glDeleteShader(shader);
        }
        return p;
    }

    void
    ShaderProgram::use(void)
    {
        glUseProgram(this->_program);
    }

    GLuint
    ShaderProgram::getAttribLocation(const char *name)
    {
        return glGetAttribLocation(this->_program, name);
    }

    GLuint
    ShaderProgram::getUniformLocation(const char *name)
    {
        return glGetUniformLocation(this->_program, name);
    }

    void
    ShaderProgram::dispose(void)
    {
        glDeleteProgram(this->_program);
    }

/* ============= */

    static const float quadvertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    static const unsigned int quadelements[] = {
        0, 1, 2,
        0, 2, 3,
    };

    const float *
    QuadGeometry::vertices(void)
    {
        return quadvertices;
    }

    const unsigned int *
    QuadGeometry::elements(void)
    {
        return quadelements;
    }

    unsigned int
    QuadGeometry::numVertices(void)
    {
        return 4;
    }

    unsigned int
    QuadGeometry::numElements(void)
    {
        return 6;
    }

    unsigned int
    QuadGeometry::numVertexComponents(void)
    {
        return 3;
    }

    unsigned int
    QuadGeometry::singleVertexSize(void)
    {
        return QuadGeometry::numVertexComponents() * sizeof(float);
    }

    unsigned int
    QuadGeometry::verticesSize(void)
    {
        return sizeof(quadvertices);
    }

    unsigned int
    QuadGeometry::elementsSize(void)
    {
        return sizeof(quadelements);
    }
}
