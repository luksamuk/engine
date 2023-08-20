#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "io.hpp"
#include "render.hpp"
#include "controls.hpp"

// TODO: This is just placeholder data!
static const float vertices[] = {
    // position2f  // color3f
    -0.6f, -0.4f,  1.0f, 0.0f, 0.0f,
     0.6f, -0.4f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.6f,  0.0f, 0.0f, 1.0f,
};

int
main(void)
{
    GLFWwindow* window = initWindow();
    initRender(window);
    initControls(window);
    
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

    static float rotationX = 0.0f;
    static float rotationY = 0.0f;
    static float rotationZ = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        if(controlsPressing(BTN_DIGITAL_UP)) {
            rotationX -= glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_DOWN)) {
            rotationX += glm::radians(1.0f);
        }

        if(controlsPressing(BTN_DIGITAL_RIGHT)) {
            rotationY -= glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_LEFT)) {
            rotationY += glm::radians(1.0f);
        }

        if(controlsPressing(BTN_DIGITAL_ACTIONUP)) {
            rotationZ -= glm::radians(1.0f);
        }
        if(controlsPressing(BTN_DIGITAL_ACTIONDOWN)) {
            rotationZ += glm::radians(1.0f);
        }

        if(controlsPressed(BTN_DIGITAL_START)) {
            rotationX = rotationY = rotationZ = 0.0f;
        }

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::rotate(glm::mat4(1.0f), rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::rotate(view, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::rotate(view, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 mvp = projection * view * model;

        glUseProgram(program);
        glUniformMatrix4fv(mvploc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(alphaloc, 0.3f + (0.2f * glm::cos(5.0f * (float)glfwGetTime())));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        processControls();
        glfwPollEvents();
    }

    disposeWindow(window);
    return 0;
}
