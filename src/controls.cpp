#include "controls.hpp"

#define UNUSED(x) (void)(x)

#include <cstring>
#include <GLFW/glfw3.h>

namespace Controls
{
    static glm::vec2 mouse_pos;
    static ButtonState current_state;
    static ButtonState old_state;

    static int kbdmap[GLFW_KEY_LAST + 1];
    static bool using_gamepad = false;

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

        if(using_gamepad) return;
    
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
        kbdmap[GLFW_KEY_Q] = BTN_DIGITAL_SHOULDERL;
        kbdmap[GLFW_KEY_E] = BTN_DIGITAL_SHOULDERR;
    }

    void
    init(GLFWwindow *window)
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
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void
    process(void)
    {
        memcpy(&old_state, &current_state, sizeof(ButtonState));
        
        // Gamepad controls
        if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
            using_gamepad = true;
            GLFWgamepadstate state;
            if(glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
                current_state.dpadup = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
                current_state.dpaddown = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
                current_state.dpadleft = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
                current_state.dpadright = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
                current_state.start = state.buttons[GLFW_GAMEPAD_BUTTON_START];
                current_state.option = state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
                current_state.actionup = state.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE];
                current_state.actiondown = state.buttons[GLFW_GAMEPAD_BUTTON_CROSS];
                current_state.actionleft = state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE];
                current_state.actionright = state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE];
                current_state.shoulderl = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
                current_state.shoulderr = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
            }
        } else {
            using_gamepad = false;
        }
    }


    bool
    pressing(DigitalButton btn)
    {
        return current_state.btn[(int)btn];
    }

    bool
    pressed(DigitalButton btn)
    {
        return current_state.btn[(int)btn] && !old_state.btn[(int)btn];
    }

    glm::vec2
    mousePos(void)
    {
        return mouse_pos;
    }

    bool
    isGamepad(void)
    {
        return using_gamepad;
    }
}
