#include "engine/core.hpp"

#include <iostream>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine/render.hpp"
#include "engine/controls.hpp"
#include "engine/scene.hpp"
#include "engine/sound.hpp"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define VERSION "0.1.0 pre-alpha"

const std::string build_date = __DATE__;
const std::string build_time = __TIME__;
const std::string build_date_and_time = __DATE__ " " __TIME__;

namespace Core
{
    static GLFWwindow *window                       = nullptr;
    static std::string _windowCaption               = "Engine";
    static std::unique_ptr<std::thread> _audiothread;

    std::string
    getVersion()
    {
        return VERSION;
    }

    std::string
    getBuildDateAndTime()
    {
        return build_date_and_time;
    }
    
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
            Sound::init();
        }

        std::cout << "Version " << VERSION << " (build " << build_date
                  << " " << build_time << ")" << std::endl;
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

        std::cout << "Starting sound control thread" << std::endl;
        _audiothread = std::make_unique<std::thread>(Sound::loop);
    
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

        std::cout << "Halting sound control thread" << std::endl;
        Sound::halt();
        _audiothread->join();
        _audiothread = nullptr;
    }

    void
    dispose(void)
    {
        if(window) {
            Scenes::Manager::dispose();
            Render::disposeWindow(window);
        }
        Sound::dispose();
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
