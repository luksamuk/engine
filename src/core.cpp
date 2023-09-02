#include "core.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render.hpp"
#include "controls.hpp"
#include "scene.hpp"

namespace Core
{
    static GLFWwindow *window = nullptr;

    void
    init(void)
    {
        if(!window) {
            window = Render::initWindow();
            Render::init(window);
            Controls::init(window);
        }
    }

    void
    gameLoop(void)
    {
        if(!window) {
            std::cerr << "Core was not initialized" << std::endl;
            return;
        }
    
        while(!glfwWindowShouldClose(window)) {
            Scenes::Manager::updateAll();

            /* draw */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            Scenes::Manager::drawAll();

            // post-drawing
            // glUseProgram(0);
            // glBindVertexArray(0);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            /* post-processing */
            glfwSwapBuffers(window);
            Controls::process();
            glfwPollEvents();
        }
    }

    void
    dispose(void)
    {
        if(window) {
            Scenes::Manager::dispose();
            Render::disposeWindow(window);
        }
    }
}
