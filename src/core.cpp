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
    static std::string _windowCaption = "Engine";

    void
    init(std::string windowCaption)
    {
        _windowCaption = windowCaption;
        Core::init();
    }
    
    void
    init(void)
    {
        if(!window) {
            window = Render::initWindow(_windowCaption);
            Render::init(window);
            Controls::init(window);
        }
    }

    void
    queryClose(void)
    {
        if(window) {
            glfwSetWindowShouldClose(window, true);
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

    void
    setWindowCaptionAppendix(std::string appendix)
    {
        if(window) {
            std::string caption = (appendix == "")
                ? _windowCaption
                : (_windowCaption + " | " + appendix);
            
            glfwSetWindowTitle(window, caption.c_str());
        }
    }
}
