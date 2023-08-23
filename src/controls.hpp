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
};

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
        };

        bool btn[10];
    };
};


void   initControls(GLFWwindow *);
void   processControls(void);


bool      controlsPressing(DigitalButton);
bool      controlsPressed(DigitalButton);
glm::vec2 controlsMousePos(void);

#endif
