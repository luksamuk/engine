#include "controls.hpp"

#define UNUSED(x) (void)(x)

#include <cstring>
#include <GLFW/glfw3.h>

static glm::vec2 mouse_pos;
static ButtonState current_state;
static ButtonState old_state;

static int kbdmap[GLFW_KEY_LAST + 1];

static void
process_keys(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mods)
{
    UNUSED(window);
    UNUSED(scancode);
    UNUSED(mods);
    
    DigitalButton btn = (DigitalButton)kbdmap[key];
    bool pressed = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
    
    if(btn >= 0) {
        current_state.btn[btn] = pressed;
    }
}

static void
process_mouse_pos(
    GLFWwindow *window,
    double xpos,
    double ypos)
{
    UNUSED(window);
    mouse_pos.x = (float)xpos;
    mouse_pos.y = (float)ypos;
}

static void
process_mouse_button(
    GLFWwindow *window,
    int button,
    int action,
    int mods)
{
    // TODO
}

void
loadDefaultMappings()
{
    kbdmap[GLFW_KEY_UP] = BTN_DIGITAL_UP;
    kbdmap[GLFW_KEY_DOWN] = BTN_DIGITAL_DOWN;
    kbdmap[GLFW_KEY_LEFT] = BTN_DIGITAL_LEFT;
    kbdmap[GLFW_KEY_RIGHT] = BTN_DIGITAL_RIGHT;
    kbdmap[GLFW_KEY_ENTER] = BTN_DIGITAL_START;
    kbdmap[GLFW_KEY_BACKSPACE] = BTN_DIGITAL_OPTION;
    kbdmap[GLFW_KEY_W] = BTN_DIGITAL_ACTIONUP;
    kbdmap[GLFW_KEY_S] = BTN_DIGITAL_ACTIONDOWN;
    kbdmap[GLFW_KEY_A] = BTN_DIGITAL_ACTIONLEFT;
    kbdmap[GLFW_KEY_D] = BTN_DIGITAL_ACTIONRIGHT;
}

void
initControls(GLFWwindow *window)
{
    memset(&current_state, 0, sizeof(ButtonState));
    memset(&old_state, 0, sizeof(ButtonState));
    memset(&kbdmap, -1, sizeof(kbdmap));
    mouse_pos = glm::vec2(0.0f, 0.0f);

    loadDefaultMappings();
    
    glfwSetKeyCallback(window, process_keys);
    glfwSetCursorPosCallback(window, process_mouse_pos);
    glfwSetMouseButtonCallback(window, process_mouse_button);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void
processControls(void)
{
    memcpy(&old_state, &current_state, sizeof(ButtonState));
}


bool
controlsPressing(DigitalButton btn)
{
    return current_state.btn[(int)btn];
}

bool
controlsPressed(DigitalButton btn)
{
    return current_state.btn[(int)btn] && !old_state.btn[(int)btn];
}
