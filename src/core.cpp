#include "core.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render.hpp"
#include "controls.hpp"
#include "scene.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
            Render::initGui(window);
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
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            Scenes::Manager::updateAll();
            Scenes::Manager::drawAll();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
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
