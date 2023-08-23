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

#include "test_scene.hpp"
#include "sprite_scene.hpp"

int
main(void)
{
    GLFWwindow* window = initWindow();
    initRender(window);
    initControls(window);

    SceneManager::add(new TestScene());
    SceneManager::add(new SpriteScene());

    // Game loop
    while(!glfwWindowShouldClose(window)) {      
        /* update */
        SceneManager::updateAll();

        /* draw */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SceneManager::drawAll();
        
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
