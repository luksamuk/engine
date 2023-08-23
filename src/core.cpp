#include "core.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render.hpp"
#include "controls.hpp"
#include "scene.hpp"

static GLFWwindow *window = nullptr;

void
initCore(void)
{
    if(!window) {
        window = initWindow();
        initRender(window);
        initControls(window);
    }
}

void
coreGameLoop(void)
{
    if(!window) {
        std::cerr << "Core was not initialized" << std::endl;
        return;
    }
    
    while(!glfwWindowShouldClose(window)) {
        SceneManager::updateAll();

        /* draw */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SceneManager::drawAll();

        // post-drawing
        // glUseProgram(0);
        // glBindVertexArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* post-processing */
        glfwSwapBuffers(window);
        processControls();
        glfwPollEvents();
    }
}

void
disposeCore(void)
{
    if(window) {
        SceneManager::dispose();
        disposeWindow(window);
    }
}
