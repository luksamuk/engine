#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "io.hpp"
#include "render.hpp"

// TODO: This is just placeholder data!
static const float vertices[] = {
    // position2f  // color3f
    -0.6f, -0.4f,  1.0f, 0.0f, 0.0f,
     0.6f, -0.4f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.6f,  0.0f, 0.0f, 1.0f,
};

static void
error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error #" << error << ": "
              << description
              << std::endl;
}

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
    std::cerr << "GL: " << message << std::endl;
}

static void
process_keys(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // TODO
}

static void
process_mouse_pos(GLFWwindow *window, double xpos, double ypos)
{
    // TODO
}

static void
process_mouse_button(GLFWwindow *window, int button, int action, int mods)
{
    // TODO
}

int
main(void)
{
    GLFWwindow* window = nullptr;

    glfwSetErrorCallback(error_callback);

    if(!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if((window = glfwCreateWindow(640, 480, "Engine", nullptr, nullptr)) == nullptr)
    {
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, process_keys);
    glfwSetCursorPosCallback(window, process_mouse_pos);
    glfwSetMouseButtonCallback(window, process_mouse_button);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    
    // Load vertex buffer
    GLuint vbo = make_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Load shaders
    const GLuint vs = load_shader("resources/shaders/default/default.vs.glsl");
    const GLuint fs = load_shader("resources/shaders/default/default.fs.glsl");
    const GLuint program = link_program(vs, fs);

    // Get shaders uniforms / attributes
    const GLint alphaloc = glGetUniformLocation(program, "alpha");
    const GLint mvploc  = glGetUniformLocation(program, "mvp");
    const GLint vposloc = glGetAttribLocation(program, "vpos");
    const GLint vcolloc = glGetAttribLocation(program, "vcol");

    glEnableVertexAttribArray(vposloc);
    glVertexAttribPointer(
        vposloc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(vcolloc);
    glVertexAttribPointer(
        vcolloc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)));

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.5f));
        
        glm::mat4 mvp = projection * view * model;

        glUseProgram(program);
        glUniformMatrix4fv(mvploc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(alphaloc, 0.5f + (0.5f * glm::cos(5.0f * (float)glfwGetTime())));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
