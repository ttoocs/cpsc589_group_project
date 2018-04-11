//Scott Saunders
//Feb 15, 2018

#pragma once

#include "types.h"

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>
#define MYGLUEMARK

namespace input {

extern bool mousePressed;
extern vec2 mousePos;

void setup(GLFWwindow* window);

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void resizeCallback(GLFWwindow* window, int width, int height);


}
