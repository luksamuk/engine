#ifndef CONTROLS_HPP_INCLUDED
#define CONTROLS_HPP_INCLUDED

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

enum DigitalButton: int {
    BTN_DIGITAL_UNKNOWN = -1,
    BTN_DIGITAL_UP = 0,
    BTN_DIGITAL_DOWN = 1,
    BTN_DIGITAL_LEFT = 2,
    BTN_DIGITAL_RIGHT = 3,
    BTN_DIGITAL_START = 4,
    BTN_DIGITAL_OPTION = 5,
    BTN_DIGITAL_ACTIONUP = 6,
    BTN_DIGITAL_ACTIONDOWN = 7,
    BTN_DIGITAL_ACTIONLEFT = 8,
    BTN_DIGITAL_ACTIONRIGHT = 9,
    BTN_DIGITAL_SHOULDERL   = 10,
    BTN_DIGITAL_SHOULDERR   = 11,
};

enum MouseButton: int {
    BTN_MOUSE_UNKNOWN = -1,
    BTN_MOUSE_LEFT    = 0,
    BTN_MOUSE_MIDDLE  = 1,
    BTN_MOUSE_RIGHT   = 2,
};

namespace Controls
{
    struct ButtonState {
        union {
            struct {
                bool dpadup;
                bool dpaddown;
                bool dpadleft;
                bool dpadright;
                bool start;
                bool option;
                bool actionup;
                bool actiondown;
                bool actionleft;
                bool actionright;
                bool shoulderl;
                bool shoulderr;
            };

            bool btn[12];
        };
    };


    void   init(GLFWwindow *);
    void   process(void);


    bool      pressing(DigitalButton);
    bool      pressed(DigitalButton);
    glm::vec2 mousePos(void);
    bool      isGamepad(void);

    bool      mousePressing(MouseButton);
    bool      mousePressed(MouseButton);
}
    
#endif
