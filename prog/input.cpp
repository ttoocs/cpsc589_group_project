//Scott Saunders
//Simple input wrapepr


#include <math.h>
#include "input.h"


namespace input{


void setup(GLFWwindow * window){
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

// handles keyboard input events
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float move = PI/200.f;

}

bool mousePressed;
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  if( (action == GLFW_PRESS) || (action == GLFW_RELEASE) )
    mousePressed = !mousePressed;
}


vec2 mousePos;
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
  int vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  //vec2 newPos = vec2(xpos/(double)vp[2], -ypos/(double)vp[3])*2.f - vec2(1.f);

//  vec2 diff = newPos - mousePos;
/*
  if(mousePressed)
    cam.rotateCamera(-diff.x, diff.y);
*/
 // mousePos = newPos;
}

}
