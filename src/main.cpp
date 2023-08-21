#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "io.hpp"
#include "render.hpp"
#include "controls.hpp"
#include "scene.hpp"

// TODO: This is just placeholder data -- represents a tetrahedron.
static const float vertices[] = {
    /* position (3f)     color (3f) */
    0.0f,  0.6f,  0.0f,  1.0f, 0.0f, 0.0f, // top
    0.6f,  -0.4f, 0.4f,  0.0f, 1.0f, 0.0f, // bottom right, close
    -0.6f, -0.4f, 0.4f,  0.0f, 0.0f, 1.0f, // bottom left, close
    0.0f,  -0.4f, -0.6f, 0.0f, 1.0f, 1.0f, // far, behind
};

static const unsigned int elements[] = {
    0, 1, 2, // front face
    0, 2, 3, // left face
    0, 1, 3, // right face
    3, 1, 2, // bottom face
};

int
main(void)
{
    GLFWwindow* window = initWindow();
    initRender(window);
    initControls(window);
    
    // Load vertices
    GLuint vbo = make_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
    GLuint ebo = make_ebo(elements, sizeof(elements), GL_STATIC_DRAW);

    // load vertex arrays
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLuint vao = make_vao();

    // Load shaders
    const GLuint vs = load_shader("resources/shaders/default/default.vs.glsl");
    const GLuint fs = load_shader("resources/shaders/default/default.fs.glsl");
    const GLuint program = link_program(vs, fs);

    // Get shaders uniforms / attributes
    glBindVertexArray(vao);
    
    const GLint alphaloc = glGetUniformLocation(program, "alpha");
    const GLint mvploc   = glGetUniformLocation(program, "mvp");
    const GLint vposloc  = glGetAttribLocation(program, "vpos");
    const GLint vcolloc  = glGetAttribLocation(program, "vcol");

    glEnableVertexAttribArray(vposloc);
    glVertexAttribPointer(
        vposloc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(vcolloc);
    glVertexAttribPointer(
        vcolloc, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Prepare matrices for model-view-projection matrix calculation
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);

    // Rotation parameters
    static float rotationX = 0.0f;
    static float rotationY = 0.0f;
    static float rotationZ = 0.0f;

    // Game loop
    while(!glfwWindowShouldClose(window)) {      
        /* update */
        SceneManager::updateAll();
        
        if(controlsPressing(BTN_DIGITAL_UP)) {
            rotationX = glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_DOWN)) {
            rotationX = glm::radians(-1.0f);
        }

        if(controlsPressing(BTN_DIGITAL_RIGHT)) {
            rotationY = glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_LEFT)) {
            rotationY = glm::radians(-1.0f);
        }

        if(controlsPressing(BTN_DIGITAL_ACTIONUP)) {
            rotationZ = glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_ACTIONDOWN)) {
            rotationZ = glm::radians(-1.0f);
        }

        if(controlsPressed(BTN_DIGITAL_START)) {
            rotationX = rotationY = rotationZ = 0.0f;
        }

        if(controlsPressed(BTN_DIGITAL_OPTION)) {
            view = glm::mat4(1.0f);
        }

        view = glm::rotate(view, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 mvp = projection * view * model;

        /* draw */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // prepare

        SceneManager::drawAll();

        // NOTE: Apparently, order matters here
        glUseProgram(program);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glUniformMatrix4fv(mvploc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(alphaloc, 0.3f + (0.2f * glm::cos(5.0f * (float)glfwGetTime())));
        
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // post-drawing
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        /* post-processing */
        glfwSwapBuffers(window);
        processControls();
        glfwPollEvents();
    }

    SceneManager::dispose();
    disposeWindow(window);
    return 0;
}
